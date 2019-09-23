sh scripts/build.sh release 

rm -rf -d sf-net

mkdir sf-net

cp bin/release/sf-net sf-net
cp -r res sf-net

echo "Deploy build created."
echo "cd sf-net to find it"