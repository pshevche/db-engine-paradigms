echo SF 1
sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen/SF1 1 1

for i in $(seq 5 5 50);
do
        echo SF $i
        sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen/SF$i 1 1
done
