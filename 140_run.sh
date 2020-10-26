       cd dbgen/
       mkdir SF140
       ./dbgen -s 140 -f
       mv *.tbl SF140/
       cd ..
       echo SF 140
#       for j in $(seq 1 1 10);
#       do
               sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen/SF140 1 1
#       done
       cd dbgen/SF140
       sudo rm *.tbl
       cd ..
       cd ..
