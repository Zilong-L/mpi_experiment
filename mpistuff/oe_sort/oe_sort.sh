mpirun --np 32 --hostfile ~/executables/host executables/oe_sort executables/256M.dat | tee logs/256M_32.log
mpirun --np 16 --hostfile ~/executables/host executables/oe_sort executables/256M.dat | tee logs/256M_16.log
mpirun --np 8 --hostfile ~/executables/host executables/oe_sort executables/256M.dat | tee logs/256M_8.log
mpirun --np 4 --hostfile ~/executables/host executables/oe_sort executables/256M.dat | tee logs/256M_4.log
mpirun --np 2 --hostfile ~/executables/host executables/oe_sort executables/256M.dat | tee logs/256M_2.log

mpirun --np 32 --hostfile ~/executables/host executables/oe_sort executables/1G.dat | tee logs/1G_32.log
mpirun --np 16 --hostfile ~/executables/host executables/oe_sort executables/1G.dat | tee logs/1G_16.log
mpirun --np 8 --hostfile ~/executables/host executables/oe_sort executables/1G.dat | tee logs/1G_8.log
mpirun --np 4 --hostfile ~/executables/host executables/oe_sort executables/1G.dat | tee logs/1G_4.log
mpirun --np 2 --hostfile ~/executables/host executables/oe_sort executables/1G.dat | tee logs/1G_2.log

mpirun --np 32 --hostfile ~/executables/host executables/oe_sort executables/4G.dat | tee logs/4G_32.log
mpirun --np 16 --hostfile ~/executables/host executables/oe_sort executables/4G.dat | tee logs/4G_16.log
mpirun --np 8 --hostfile ~/executables/host executables/oe_sort executables/4G.dat | tee logs/4G_8.log
mpirun --np 4 --hostfile ~/executables/host executables/oe_sort executables/4G.dat | tee logs/4G_4.log
mpirun --np 2 --hostfile ~/executables/host executables/oe_sort executables/4G.dat | tee logs/4G_2.log
