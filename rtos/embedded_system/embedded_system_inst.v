	embedded_system u0 (
		.buttons_export  (<connected-to-buttons_export>),  // buttons.export
		.clk_clk         (<connected-to-clk_clk>),         //     clk.clk
		.leds_export     (<connected-to-leds_export>),     //    leds.export
		.reset_reset_n   (<connected-to-reset_reset_n>),   //   reset.reset_n
		.to_hex_readdata (<connected-to-to_hex_readdata>), //  to_hex.readdata
		.meas_export     (<connected-to-meas_export>)      //    meas.export
	);

