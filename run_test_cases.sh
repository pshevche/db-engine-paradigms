for i in $(seq 1 1 10)
do
	sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen/SF1 1 1
done

for i in $(seq 5 5 50);
do
#	cd dbgen/
#	sudo rm -rf cached
#	mkdir SF$i
#	./dbgen -s $i -f
#	mv *.tbl SF$i/
#	cd ..
#	echo SF $i
	for j in $(seq 1 1 10);
	do
		sudo ./run_hybrid 1 /home/gurumurt/db-engine-paradigms/dbgen/SF$i 1 1
	done
done
