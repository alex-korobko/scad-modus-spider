#!/bin/sh
#
# Compiles sample DDL files
#
# eXtremeDB version 2.2
# 	
# Copyright (C) 2001-2003 McObject LLC. All Rights Reserved.
#
#

echo "Compiling samples with eXtremeDB Schema Compiler..."
MCOFILES=`find ./|grep '.mco$'|cut -d/ -f1,2`
for fn in $MCOFILES
do
	cd $fn
	/opt/eXtremeDB/host/bin/mcocomp -o -c *.mco
	cd ..
done
echo "Sample interfaces generated"

#
# end-of-file $
#






