
module embedded_system (
	buttons_buttons_conduit,
	clk_clk,
	leds_export,
	reset_reset_n,
	to_hex_readdata,
	meas_export);	

	input	[1:0]	buttons_buttons_conduit;
	input		clk_clk;
	output	[9:0]	leds_export;
	input		reset_reset_n;
	output	[47:0]	to_hex_readdata;
	output	[9:0]	meas_export;
endmodule
