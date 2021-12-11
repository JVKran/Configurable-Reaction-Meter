
module embedded_system (
	buttons_buttons_conduit,
	clk_clk,
	leds_leds_conduit,
	reset_reset_n,
	to_hex_readdata);	

	input	[1:0]	buttons_buttons_conduit;
	input		clk_clk;
	output	[9:0]	leds_leds_conduit;
	input		reset_reset_n;
	output	[47:0]	to_hex_readdata;
endmodule
