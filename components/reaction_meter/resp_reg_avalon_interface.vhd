LIBRARY ieee;
USE ieee.std_logic_1164.all;

-- Our reaction meter has two registers; one with the state and one with the
-- response time. Hence, it has an address, read and readdata line. The registers
-- can't be written, since actions only depend on the presses of buttons. Furthermore,
-- it directly controls 10 leds and reads 2 buttons. According to the documentation at 
-- page 14, byteenable can be ignored. Chipselect is also deprecated.


ENTITY resp_reg_avalon_interface IS
	PORT ( clock, resetn 			: IN STD_LOGIC;
		read, address, chipselect 	: IN STD_LOGIC;
		irq								: OUT STD_LOGIC;
		readdata 						: OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		leds 								: OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
		buttons 							: IN STD_LOGIC_VECTOR(1 DOWNTO 0)
	);
END resp_reg_avalon_interface;

ARCHITECTURE structure OF resp_reg_avalon_interface IS
	SIGNAL from_reg 			: STD_LOGIC_VECTOR(15 DOWNTO 0);
	
	COMPONENT resp_reg
		PORT ( 	clock, resetn, address : IN STD_LOGIC;
				data 				: OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
				read				: IN STD_LOGIC;
				irq				: OUT STD_LOGIC;
				leds 				: OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
				buttons 			: IN STD_LOGIC_VECTOR(1 DOWNTO 0)
		);
	END COMPONENT;
BEGIN
	WITH (chipselect AND read) SELECT
		readdata <= from_reg WHEN '1', (others => '1') WHEN OTHERS;
		
	reg_instance: resp_reg PORT MAP (clock, resetn, address, from_reg, read, irq, leds, buttons);
END structure;