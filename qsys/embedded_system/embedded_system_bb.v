
module embedded_system (
	clk_clk,
	reset_reset_n,
	leds_export,
	buttons_export,
	to_hex_readdata);	

	input		clk_clk;
	input		reset_reset_n;
	output	[9:0]	leds_export;
	input	[3:0]	buttons_export;
	output	[47:0]	to_hex_readdata;
endmodule
