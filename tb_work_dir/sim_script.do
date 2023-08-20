vlog sha256.v sha256_core.v sha256_k_constants.v sha256_w_mem.v testbench.sv model.c
vsim -voptargs=+acc +cover work.tb_sha256
run -all
quit -sim
