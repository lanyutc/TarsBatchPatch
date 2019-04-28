#include <string>
#include <iostream>
#include "servant/Communicator.h"
#include "util/tc_option.h"
#include "AdminReg.h"

using namespace tars;
using namespace std;

void usage(char *argv[])
{
    cout << "Usage:" << argv[0] 
		<< " --adminreg=[adminreg] --app=[appname] --servername=[servername] --serverip=[serverip] --patchversion=[version]" << endl;
    cout << "adminreg: tars.tarsAdminRegistry.AdminRegObj@tcp -h ip -p port" << endl;
    cout << "app: tars app name" << endl;
    cout << "servername: tars server name" << endl;
    cout << "serverip: tars server local ip" << endl;
	cout << "patchversion: patch version" << endl; 
}


int main(int argc, char *argv[])
{
	TC_Option option;
	option.decode(argc, argv);
	if (!option.hasParam("adminreg") || !option.hasParam("app") || !option.hasParam("servername") 
	|| !option.hasParam("serverip") || !option.hasParam("patchversion")) {
		usage(argv);
		exit(0);
	}

	Communicator comm;
	AdminRegPrx prx;
	try
	{
		comm.stringToProxy(option.getValue("adminreg") , prx);

		string app = option.getValue("app");
		string server = option.getValue("servername");
		string targetIp = option.getValue("serverip");

		//stop server
		string result;
		int code = prx->tars_set_timeout(5000)->stopServer(app, server, targetIp, result);
		cout << targetIp << "|stop server code|" << code << "|result:" << result << endl;
		if (code != 0) {
			exit(-1);
		}

		//patch server
		PatchRequest req;
		req.appname = app;
		req.servername = server;
		req.nodename = targetIp;
		req.version = option.getValue("patchversion");
		req.patchobj = "tars.tarspatch.PatchObj";
		result.clear();
		code = prx->tars_set_timeout(5000)->batchPatch(req, result);
		cout << targetIp << "|patch server code|" << code << "|result:" << result << endl;
		if (code != 0) {
			exit(-2);
		}

		//query patch percent
		PatchInfo tPatchInfo;
		for (size_t i = 0; i < 10; i++) {
			sleep(1);
			code = prx->tars_set_timeout(5000)->getPatchPercent(app, server, targetIp, tPatchInfo);
			cout << targetIp << "|percent code|" << code << "|" << tPatchInfo.iPercent << "|bSucc:" << (tPatchInfo.bSucc ? 1 : 0) << endl;
			if (tPatchInfo.iPercent == 100 && tPatchInfo.bSucc) {
				break;
			}
		}

		//patch failed
		if (!tPatchInfo.bSucc) {
			cerr << targetIp << "|patch failed|-1|" << tPatchInfo.iPercent << endl;
			exit(-3);
		}

		//start server
		result.clear();
		code = prx->tars_set_timeout(5000)->startServer(app, server, targetIp, result);
		cout << targetIp << "|start server code|" << code << "|result:" << result << endl;
		if (code != 0) {
			exit(-4);
		}
	}
	catch(exception& e)
	{
		cerr << "exception:" << e.what() << endl;
	}
	catch (...)
	{
		cerr << "unknown exception" << endl;
	}
	return 0;
}
