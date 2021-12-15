LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.ALL;


ENTITY qsys_reaction_meter IS
		port (
            KEY    				: IN  STD_LOGIC_VECTOR(3 downto 0)  := (others => 'X');
            CLOCK_50          : IN  STD_LOGIC                     := 'X';
				HEX0 					: OUT STD_LOGIC_VECTOR(0 TO 6);
				HEX1 					: OUT STD_LOGIC_VECTOR(0 TO 6);
				HEX2 					: OUT STD_LOGIC_VECTOR(0 TO 6);
				HEX3 					: OUT STD_LOGIC_VECTOR(0 TO 6);
				HEX4 					: OUT STD_LOGIC_VECTOR(0 TO 6);
				HEX5 					: OUT STD_LOGIC_VECTOR(0 TO 6);
				LEDR       			: OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
            SW 					: IN STD_LOGIC_VECTOR(0 DOWNTO 0);
				GPIO_1				: OUT STD_LOGIC_VECTOR(9 DOWNTO 0)
        );
END qsys_reaction_meter;

ARCHITECTURE structure OF qsys_reaction_meter IS
    component embedded_system is
        port (
            buttons_buttons_conduit : in  std_logic_vector(1 downto 0)  := (others => 'X'); -- export
            clk_clk           		: in  std_logic                     := 'X';             -- clk
            to_hex_readdata 			: out std_logic_vector(47 downto 0);                    -- readdata
            leds_export 				: out std_logic_vector(9 downto 0);                     -- export
            reset_reset_n     		: in  std_logic                     := 'X';             -- reset_n
				meas_export             : out std_logic_vector(9 downto 0)  
        );
    end component embedded_system;
	 
BEGIN

    u0 : embedded_system
        port map (
            buttons_buttons_conduit(0)		=> KEY(0),
				buttons_buttons_conduit(1)		=> KEY(1),
            clk_clk           				=> CLOCK_50,
				to_hex_readdata(6 DOWNTO 0) 	=> HEX0,
				to_hex_readdata(13 DOWNTO 7) 	=> HEX1,
				to_hex_readdata(20 DOWNTO 14) => HEX2,
				to_hex_readdata(27 DOWNTO 21) => HEX3,
				to_hex_readdata(37 DOWNTO 31) => HEX4,
				to_hex_readdata(44 DOWNTO 38) => HEX5,
            leds_export       				=> LEDR,
            reset_reset_n     				=> SW(0),
				meas_export							=> GPIO_1
        );
END structure;