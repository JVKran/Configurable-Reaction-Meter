	embedded_system u0 (
		.clk_clk         (<connected-to-clk_clk>),         //     clk.clk
		.reset_reset_n   (<connected-to-reset_reset_n>),   //   reset.reset_n
		.leds_export     (<connected-to-leds_export>),     //    leds.export
		.buttons_export  (<connected-to-buttons_export>),  // buttons.export
		.to_hex_readdata (<connected-to-to_hex_readdata>)  //  to_hex.readdata
	);

