# in the container: bash /tk/app/icons/test/folder.sh : where the symbols are looked for, by what dladdr says
cd /tmp/icons/test; mkdir -p cwd; rm -rf cwd/symbols; cp -r /tk/app/icons/test/files/symbols cwd/symbols
export HOME=/tmp/icons/test/home FAKE_HOME=/tmp/icons/test/home LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8
cd cwd
for f in fail null plain.so "" / // /x.so cwd/x.so /tmp/icons/test/cwd/x.so ../cwd/x.so /tmp/icons/test/cwd/ ./x.so /no/such/x.so; do
  echo "== $f"; FAKE_DLADDR="$f" LD_PRELOAD=/tmp/icons/test/malloc.so timeout 100 ../driver /tmp/icons/test/t folder
done
