module tb_sha256();

	typedef bit [31:0] uint8_t;	
	
	import "DPI-C" function void test1(int dut_in[16], uint8_t dut_res[8]);
	import "DPI-C" function void test2(int dut_in[16], uint8_t dut_res[8]);

	//----------------------------------------------------------------
	// Parameter and Clock Definitions
	//----------------------------------------------------------------
	parameter CLK_HALF_PERIOD = 2;
	parameter CLK_PERIOD = 2 * CLK_HALF_PERIOD;

	//----------------------------------------------------------------
	// Address Definitions
	//----------------------------------------------------------------
	parameter ADDR_NAME0 = 8'h00;
	parameter ADDR_BLOCK0    = 8'h10;
  parameter ADDR_BLOCK1    = 8'h11;
  parameter ADDR_BLOCK2    = 8'h12;
  parameter ADDR_BLOCK3    = 8'h13;
  parameter ADDR_BLOCK4    = 8'h14;
  parameter ADDR_BLOCK5    = 8'h15;
  parameter ADDR_BLOCK6    = 8'h16;
  parameter ADDR_BLOCK7    = 8'h17;
  parameter ADDR_BLOCK8    = 8'h18;
  parameter ADDR_BLOCK9    = 8'h19;
  parameter ADDR_BLOCK10   = 8'h1a;
  parameter ADDR_BLOCK11   = 8'h1b;
  parameter ADDR_BLOCK12   = 8'h1c;
  parameter ADDR_BLOCK13   = 8'h1d;
  parameter ADDR_BLOCK14   = 8'h1e;
  parameter ADDR_BLOCK15   = 8'h1f;


	//----------------------------------------------------------------
	// Register and Wire declarations.
	//----------------------------------------------------------------
	reg [31:0] cycle_ctr;
	reg [31:0] error_ctr;
	reg [31:0] tc_ctr;
	reg tb_clk;
	reg tb_reset_n;
	reg tb_cs;
	reg tb_we;
	reg [7:0] tb_address;
	reg [31:0] tb_write_data;
	wire [31:0] tb_read_data;
	wire tb_error;
	reg [31:0] read_data;
	bit [31:0] digest_data[8];

	//----------------------------------------------------------------
	// Device Under Test (DUT) Instantiation
	//----------------------------------------------------------------
	sha256 dut(
		.clk(tb_clk),
		.reset_n(tb_reset_n),
		.cs(tb_cs),
		.we(tb_we),
		.address(tb_address),
		.write_data(tb_write_data),
		.read_data(tb_read_data),
		.error(tb_error)
	);
     parameter ADDR_DIGEST0   = 8'h20;
  parameter ADDR_DIGEST1   = 8'h21;
  parameter ADDR_DIGEST2   = 8'h22;
  parameter ADDR_DIGEST3   = 8'h23;
  parameter ADDR_DIGEST4   = 8'h24;
  parameter ADDR_DIGEST5   = 8'h25;
  parameter ADDR_DIGEST6   = 8'h26;
  parameter ADDR_DIGEST7   = 8'h27;
	//----------------------------------------------------------------
	// Coverage Group
	//----------------------------------------------------------------
	covergroup c_group @(negedge tb_clk);
		cp1: coverpoint tb_address {bins b1[] = {[16 : 30],8,9};
				    }
	cp2: coverpoint tb_write_data {option.auto_bin_max = 10;bins b2[] = {[32'h0000ffff:32'h0f0f0f0f]}; 
				    }
	cp3: coverpoint tb_read_data{option.auto_bin_max = 10;bins b3[] = {[32'h0000ffff:32'h0f0f0f0f]};
                                     }
	cp4: coverpoint tb_cs {bins b4[] = {1,0};
				  }
	
	cp5: coverpoint tb_error {bins b5[] = {1,0};
				}
	cp6 : cross cp1,cp2 {  bins addr_wr_data = binsof(cp1.b1[16:30]);
			       bins addr2_wr_data = binsof(cp1.b1[16]);
			       bins addr3_wrd_data = binsof(cp1.b1[17]);
				}
	endgroup
	c_group c_inst;
	
  parameter ADDR_NAME1       = 8'h01;
  parameter ADDR_VERSION     = 8'h02;

  parameter ADDR_CTRL        = 8'h08;
  parameter CTRL_INIT_VALUE  = 8'h01;
  parameter CTRL_NEXT_VALUE  = 8'h02;
  parameter CTRL_MODE_VALUE  = 8'h04;

  parameter ADDR_STATUS      = 8'h09;
  parameter STATUS_READY_BIT = 0;
  parameter STATUS_VALID_BIT = 1;



	always begin : clk_gen
		#CLK_HALF_PERIOD tb_clk = !tb_clk;
	end


	always begin : sys_monitor
		#(2 * CLK_HALF_PERIOD);
		cycle_ctr = cycle_ctr + 1;
	end
	  task reset_dut;
    begin
      $display("*** Toggle reset.");
      tb_reset_n = 0;
      #(4 * CLK_HALF_PERIOD);
      tb_reset_n = 1;
    end
  endtask // reset_dut


 
  task init_sim;
    begin
      cycle_ctr = 32'h0;
      error_ctr = 32'h0;
      tc_ctr = 32'h0;

      tb_clk = 0;
      tb_reset_n = 0;
      tb_cs = 0;
      tb_we = 0;
      tb_address = 6'h0;
      tb_write_data = 32'h0;
    end
  endtask // init_dut


  
  task wait_ready;
    begin
      read_data = 0;

      while (read_data == 0)
        begin
          read_word(ADDR_STATUS);
        end
    end
  endtask // wait_ready



  task write_word(input [7 : 0]  address,
                  input [31 : 0] word);
    begin
      if (DEBUG)
        begin
          $display("*** Writing 0x%08x to 0x%02x.", word, address);
          $display("");
        end

      tb_address = address;
      tb_write_data = word;
      tb_cs = 1;
      tb_we = 1;
      #(CLK_PERIOD);
      tb_cs = 0;
      tb_we = 0;
    end
  endtask // write_word

 task write_block(input int  block[0:15]);
    begin
      write_word(ADDR_BLOCK0,  block[0]);
      write_word(ADDR_BLOCK1,  block[1]);
      write_word(ADDR_BLOCK2,  block[2]);
      write_word(ADDR_BLOCK3,  block[3]);
      write_word(ADDR_BLOCK4,  block[4]);
      write_word(ADDR_BLOCK5,  block[5]);
      write_word(ADDR_BLOCK6,  block[6]);
      write_word(ADDR_BLOCK7,  block[7]);
      write_word(ADDR_BLOCK8,  block[8]);
      write_word(ADDR_BLOCK9,  block[9]);
      write_word(ADDR_BLOCK10, block[10]);
      write_word(ADDR_BLOCK11, block[11]);
      write_word(ADDR_BLOCK12, block[12]);
      write_word(ADDR_BLOCK13, block[13]);
      write_word(ADDR_BLOCK14, block[14]);
      write_word(ADDR_BLOCK15, block[15]);
    end
  endtask // write_block



  task read_word(input [7 : 0]  address);
    begin
      tb_address = address;
      tb_cs = 1;
      tb_we = 0;
      #(CLK_PERIOD);
      read_data = tb_read_data;
      tb_cs = 0;

      if (DEBUG)
        begin
          $display("*** Reading 0x%08x from 0x%02x.", read_data, address);
          $display("");
        end
    end
  endtask // read_word



  task read_digest;
    begin
      read_word(ADDR_DIGEST0);
      digest_data[0] = read_data;
      read_word(ADDR_DIGEST1);
      digest_data[1] = read_data;
      read_word(ADDR_DIGEST2);
      digest_data[2] = read_data;
      read_word(ADDR_DIGEST3);
      digest_data[3] = read_data;
      read_word(ADDR_DIGEST4);
      digest_data[4] = read_data;
      read_word(ADDR_DIGEST5);
      digest_data[5] = read_data;
      read_word(ADDR_DIGEST6);
      digest_data[6] = read_data;
      read_word(ADDR_DIGEST7);
      digest_data[7] = read_data;
    end
  endtask // read_digest

	// Other tasks...

	//----------------------------------------------------------------
	// sha256_tests Task
	//----------------------------------------------------------------
	task sha256_tests(int seed,int i);
    begin : sha256_tests_block
      int tc0[16];
      reg [255 : 0] res0;
      bit mode;
      reg [511:0] tci;
      reg [511 : 0] tc1_0;
      reg [255 : 0] res1_0;
      reg [511 : 0] tc1_1;
      reg [255 : 0] res1_1;

      $display("*** Testcases for sha256 functionality started.");
	////////////////////////////////////////////////////// TESTCASE 1 ////////////////////////////////////////////////////////	
	 $display("*** TC%01d - Single block test started.", tc_ctr);
	
	////////////////////////////////////////////////////// INPUT PATTERN /////////////////////////////////////////////////////////
        if(i == 1)
	begin
		for(int i = 0; i<16 ; i = i+1)
		begin
	 	tc0[i] =  $urandom(seed);
		end
	end
	else if(i == 2)
	begin
		for(int i = 0; i<16 ; i = i+1)
		begin
		tc0[i] = 'b0;
		end
	
	end
	else if(i == 3)
	begin
		for(int i = 0; i< 16 ; i=i+1)
		begin
		tc0[i] = 32'hffffffff;
		end
	
	end

	mode = SHA256_MODE;
      	////////////////////////////////////////////////WRITING BLOCK TO DUT ////////////////////////////////////////////////////////
	write_block(tc0);
	

      	if (mode)
        	write_word(ADDR_CTRL, (CTRL_MODE_VALUE + CTRL_INIT_VALUE));
      	else
        	write_word(ADDR_CTRL, CTRL_INIT_VALUE);

      	#(CLK_PERIOD);
      	wait_ready();
	/////////////////////////////////////////////////////// READ FROM DUT in digest_data //////////////////////////////////////////
      	read_digest();


 
	$display("The print data is %0p",digest_data);
	////////////////////////////////////////////////////////// C MODE TO CAPTURE AND COMPRE THE DATA /////////////////////////////
     	test1(tc0,digest_data);
 end
endtask // sha256_tests

	//----------------------------------------------------------------
	// Main
	//----------------------------------------------------------------
	initial begin : main
		int k;
		c_inst = new;
		c_inst.start();
		init_sim();
		c_inst.sample();
		reset_dut();
		c_inst.sample();

		// Testcase 1 with random stimuli
		for (int i = 0; i < 1000; i++) begin
			k = $urandom();
			sha256_tests(k, 1);
			c_inst.sample();
		end

		// Testcase 2
		begin
			k = $urandom();
			sha256_tests(k, 2);
			c_inst.sample();
		end

		// Testcase 3
		begin
			k = $urandom();
			sha256_tests(k, 3);
			c_inst.sample();
		end

		c_inst.stop();
		$display("   -- Testbench for sha256 done. --");
		$finish;
	end
endmodule
