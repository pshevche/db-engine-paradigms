        cd dbgen/
        sudo rm -rf cached
        ./dbgen -s 1 -f
        cd ..
        echo SF 1
        make && sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen 1 1
