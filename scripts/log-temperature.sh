#/bin/sh -e

probe_path=$1
probe_name=$2

api_endpoint="http://localhost:3000/api/probe/$probe_name/push"

while [ -z $temp ]; do
    temp=`awk 'BEGIN { FS="[ =]" } /YES/ { valid=1 }; /t=([0-9]*)/ { if (valid) print $11/1000 }' $probe_path`
    sleep 1
done

wget -q --post-data="" $api_endpoint/$temp

