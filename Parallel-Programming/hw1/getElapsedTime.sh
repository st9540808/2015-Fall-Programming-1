> $1.data
for N in {0..4}; do
    printf "$((2 ** N)) " >> $1.data
    mpiexec -n $((2 ** N)) $1 $2 | grep "Process 0" | grep -o -P '(?<=in\ ).*(?=secs)' >> $1.data;
done
