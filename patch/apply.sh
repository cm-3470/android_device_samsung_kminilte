#!/bin/sh

MYABSPATH=$(readlink -f "$0")
PATCHBASE=$(dirname "$MYABSPATH")
CMBASE=$(readlink -f "$PATCHBASE/../../../../")

for i in $(find "$PATCHBASE"/* -type d); do
	PATCHNAME=$(basename "$i")
	PATCHTARGET=$PATCHNAME
	for j in $(seq 4); do
		PATCHTARGET=$(echo $PATCHTARGET | sed 's/_/\//')
		if [ -d "$CMBASE/$PATCHTARGET" ]; then break; fi
	done
	
	if [ "$PATCHTARGET" = "bootable/recovery" ]; then
        # recovery might be replaced by TWRP -> only patch if CM recovery is used
        cd $CMBASE/bootable/recovery
        recovery_url=`git config --get remote.github.url`
        cd -
        if [ ! "$recovery_url" = "https://github.com/CyanogenMod/android_bootable_recovery" ]; then
            echo "WARN: Recovery does not seem to be CM one -> patches will not be applied"
            continue
        fi
    fi    
	
	echo applying $PATCHNAME to $PATCHTARGET
	cd "$CMBASE/$PATCHTARGET" || exit 1
	git am --ignore-whitespace -3 "$PATCHBASE/$PATCHNAME"/* || exit 1
done
