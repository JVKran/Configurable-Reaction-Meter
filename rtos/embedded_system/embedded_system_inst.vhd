	component embedded_system is
		port (
			buttons_export  : in  std_logic_vector(3 downto 0)  := (others => 'X'); -- export
			clk_clk         : in  std_logic                     := 'X';             -- clk
			to_hex_readdata : out std_logic_vector(47 downto 0);                    -- readdata
			leds_export     : out std_logic_vector(9 downto 0);                     -- export
			reset_reset_n   : in  std_logic                     := 'X'              -- reset_n
		);
	end component embedded_system;

	u0 : component embedded_system
		port map (
			buttons_export  => CONNECTED_TO_buttons_export,  -- buttons.export
			clk_clk         => CONNECTED_TO_clk_clk,         --     clk.clk
			to_hex_readdata => CONNECTED_TO_to_hex_readdata, --  to_hex.readdata
			leds_export     => CONNECTED_TO_leds_export,     --    leds.export
			reset_reset_n   => CONNECTED_TO_reset_reset_n    --   reset.reset_n
		);

