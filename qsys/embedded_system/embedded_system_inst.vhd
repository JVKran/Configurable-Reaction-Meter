	component embedded_system is
		port (
			clk_clk         : in  std_logic                     := 'X';             -- clk
			reset_reset_n   : in  std_logic                     := 'X';             -- reset_n
			leds_export     : out std_logic_vector(9 downto 0);                     -- export
			buttons_export  : in  std_logic_vector(3 downto 0)  := (others => 'X'); -- export
			to_hex_readdata : out std_logic_vector(47 downto 0)                     -- readdata
		);
	end component embedded_system;

	u0 : component embedded_system
		port map (
			clk_clk         => CONNECTED_TO_clk_clk,         --     clk.clk
			reset_reset_n   => CONNECTED_TO_reset_reset_n,   --   reset.reset_n
			leds_export     => CONNECTED_TO_leds_export,     --    leds.export
			buttons_export  => CONNECTED_TO_buttons_export,  -- buttons.export
			to_hex_readdata => CONNECTED_TO_to_hex_readdata  --  to_hex.readdata
		);

