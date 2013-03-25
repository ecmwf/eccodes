host=$(hostname)
host=%HOST%
package=%PACKAGE%
root=%P4ROOT%/client/main
[[ ! -d $root ]] && mkdir -p $root
cd $root
export P4CLIENT=${package}_${host}

export P4PORT=p4od:14001

p4login
p4 client -t ${package}_template  -o | p4 client -i

p4 sync %P4SYNCOPTION% 
make .init             # creates make.dep

