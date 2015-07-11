#!/bin/sh

dir=(game gate filter dbase)
common_dir=(net common game_def instance)

for var in ${common_dir[@]};
do
	cd $var;
	svn up;
	cd ..;
done

for var in ${dir[@]};
do
	cd $var;
	svn up;
	make -j 2;
	cd ..;
done
