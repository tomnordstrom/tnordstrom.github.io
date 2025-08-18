#!/bin/bash

function netmask2cidr
{ 
  case $1 in
      0x*)
      local hex=${1#0x*} quad=
      while [ -n "${hex}" ]; do
        local lastbut2=${hex#??*}
        quad=${quad}${quad:+.}0x${hex%${lastbut2}*}
        hex=${lastbut2}
      done
      set -- ${quad}
      ;;
  esac

  local i= len=
  local IFS=.
  for i in $1; do
    while [ ${i} != "0" ]; do
      len=$((${len} + ${i} % 2))
      i=$((${i} >> 1))
    done
  done

  echo "${len}"
}
