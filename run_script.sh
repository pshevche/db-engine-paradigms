for i in $(seq 100 10 200);
do
        echo SF $i
#        for j in $(seq 1 1 10);
#        do
                sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen/SF$i 1 1
#        done
done
#	echo SF 250
#		sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen/SF250 1 1
#for i in $(seq 210 10 240);
#do
#	cd dbgen/
#	mkdir SF$i
#	./dbgen -s $i -f
#	mv *.tbl SF$i/
#	cd ..
#	echo SF $i
#	for j in $(seq 1 1 10);
#	do
#		sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen/SF$i 1 1
#	done
#	cd dbgen/SF$i
#	sudo rm *.tbl
#	cd ..
#	cd ..
#done
