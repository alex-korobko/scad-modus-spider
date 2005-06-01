#!/bin/sh

#variables
typeset -i upo_i=$1
typeset -i ie1_i=$2
typeset -i e1_i=$3
typeset -i channelb_i=$4
#constants
typeset -i upo_count=4
typeset -i ie1_count=16
typeset -i e1_count=16
typeset -i channelb_count=32
#expressions
let upo_distance=2*ie1_count+2*ie1_count*e1_count+2*ie1_count*e1_count*channelb_count+1
let ie1_distance=2*e1_count+2*e1_count*channelb_count+1
let e1_distance=2*channelb_count+1
let chb_distance=1

let upo_left_side=upo_i*upo_distance+upo_i+1
let upo_right_side=upo_left_side+upo_distance

let ie1_left_side=upo_i*upo_distance+ie1_i*ie1_distance+upo_i+ie1_i+2
let ie1_right_side=ie1_left_side+ie1_distance

let e1_left_side=upo_i*upo_distance+ie1_i*ie1_distance+e1_i*e1_distance+upo_i+ie1_i+e1_i+3
let e1_right_side=e1_left_side+e1_distance

let chb_left_side=upo_i*upo_distance+ie1_i*ie1_distance+e1_i*e1_distance+channelb_i*chb_distance+upo_i+ie1_i+e1_i+channelb_i+4
let chb_right_side=chb_left_side+chb_distance
#outputs
echo "============================"
echo "Inputs:"
echo "========"
echo "upo_i $upo_i"
echo "ie1_i $ie1_i"
echo "e1_i $e1_i"
echo "channelb_i $channelb_i"
echo "============================"
echo "Results:"
echo "========"
echo "upo left side $upo_left_side"
echo "upo right side $upo_right_side"
echo "upo sides distance $(($upo_right_side-$upo_left_side))"
echo "========"
echo "ie1 left side $ie1_left_side"
echo "ie1 right side $ie1_right_side"
echo "ie1 sides distance $(($ie1_right_side-$ie1_left_side))"
echo "========"
echo "e1 left side $e1_left_side"
echo "e1 right side $e1_right_side"
echo "e1 sides distance $(($e1_right_side-$e1_left_side))"
echo "========"
echo "channelb left side $chb_left_side"
echo "channelb right side $chb_right_side"
echo "channelb sides distance $(($chb_right_side-$chb_left_side))"
echo "========"
