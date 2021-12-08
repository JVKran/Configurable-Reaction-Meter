
module embedded_system (
	buttons_export,
	clk_clk,
	to_hex_readdata,
	leds_export,
	reset_reset_n);	

	input	[3:0]	buttons_export;
	input		clk_clk;
	output	[47:0]	to_hex_readdata;
	output	[9:0]	leds_export;
	input		reset_reset_n;
endmodule
