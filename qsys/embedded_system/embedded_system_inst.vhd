	component embedded_system is
		port (
			buttons_buttons_conduit : in  std_logic_vector(1 downto 0)  := (others => 'X'); -- buttons_conduit
			clk_clk                 : in  std_logic                     := 'X';             -- clk
			leds_export             : out std_logic_vector(9 downto 0);                     -- export
			reset_reset_n           : in  std_logic                     := 'X';             -- reset_n
			to_hex_readdata         : out std_logic_vector(47 downto 0);                    -- readdata
			meas_export             : out std_logic_vector(9 downto 0)                      -- export
		);
	end component embedded_system;

	u0 : component embedded_system
		port map (
			buttons_buttons_conduit => CONNECTED_TO_buttons_buttons_conduit, -- buttons.buttons_conduit
			clk_clk                 => CONNECTED_TO_clk_clk,                 --     clk.clk
			leds_export             => CONNECTED_TO_leds_export,             --    leds.export
			reset_reset_n           => CONNECTED_TO_reset_reset_n,           --   reset.reset_n
			to_hex_readdata         => CONNECTED_TO_to_hex_readdata,         --  to_hex.readdata
			meas_export             => CONNECTED_TO_meas_export              --    meas.export
		);

