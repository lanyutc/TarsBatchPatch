#/bin/bash
#example:sh patch_server.sh TestServer 10
#check err:awk -F '|' '{if(NF>3 && $3!=0) print $0}' patch.log

if [[ $# -ne 2 ]]; then
    echo "usage:$0 server patch_version"
    exit 2
fi

cd $(dirname $0)

echo `pwd` > patch.log

./PatchClient --adminreg='tars.tarsAdminRegistry.AdminRegObj@tcp -h 10.1.0.1 -p 12000' --app=test --servername=$1 --serverip=10.1.0.1 --patchversion=$2 >> patch.log
./PatchClient --adminreg='tars.tarsAdminRegistry.AdminRegObj@tcp -h 10.1.0.1 -p 12000' --app=test --servername=$1 --serverip=10.1.0.2 --patchversion=$2 >> patch.log
