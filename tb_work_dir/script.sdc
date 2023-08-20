################################################################################
# Timing constraint
################################################################################
create_clock -period 5.0 [get_ports clk]

# Input delay and output delay
set_input_delay -max 1.0 -clock clk \
  [remove_from_collection [all_inputs] [get_ports clk]]



set_input_delay -clock [get_port clk] -max 0.5 [get_port write_data]
set_input_delay -clock [get_port clk] -min 0.3 [get_port write_data]

set_input_delay -clock [get_port clk] -max 0.7 [get_port address]
set_input_delay -clock [get_port clk] -min 0.5 [get_port address]

set_input_delay -clock [get_port clk] -max 0.2 [get_port clk]

set_output_delay -max 0.3 -clock clk [get_port read_data]
set_output_delay -min 0.2 -clock clk [get_port read_data]
set_output_delay -clock [get_port clk] -max 0.1 [get_port error]
set_output_delay -clock [get_port clk] -max 0.1 [get_port error]

################################################################################
# Enviornement attribute constraint
################################################################################
# Load on the output ports
set_load 1 [all_outputs]

# Input transition time on all inputs except clock
set_input_transition 0.1 \
  [remove_from_collection [all_inputs] [get_ports clk]]
