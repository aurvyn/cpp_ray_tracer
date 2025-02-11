#!/bin/bash
prog=../build/tracer
resList="100 200 300"
sceneList=`ls ./*obj`

function comparePerformance()
{
	scene=$1
	for res in $resList
	do
		output=$( ( time -p $prog -res $res $res $scene $scene.png) )
		realTimeVal=`echo $output | sed s/.*real\ // | sed s/\ user\ .*//`
		echo $scene $res $realTimeVal tree

		output=$( ( time -p $prog -l -res $res $res $scene) 2>&1 | tee)
		realTimeVal=`echo $output | sed s/.*real\ // | sed s/\ user\ .*//`
		echo $scene $res $realTimeVal array
	done
}

function saveImage()
{
	scene=$1
	res=100
	echo $scene $res
	output=$( ( time -p $prog -res $res $res $scene $scene.png) )
}



for scene in $sceneList
do
	saveImage $scene
done
