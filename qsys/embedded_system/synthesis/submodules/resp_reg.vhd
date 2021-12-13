LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY resp_reg IS
	PORT ( 	clock, resetn, address 	: IN STD_LOGIC;
			data 								: OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
			read								: IN STD_LOGIC;
			irq								: OUT STD_LOGIC;
			leds 								: OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
			buttons 							: IN STD_LOGIC_VECTOR(1 DOWNTO 0)
	);
END resp_reg;

ARCHITECTURE behavior of resp_reg IS

	SIGNAL prev_buttons : STD_LOGIC_VECTOR(1 DOWNTO 0);
	
	-- With chipselect and read from-reg doorkoppelen aan read-data.
	-- Read lijntje doorgeven en irq resetten als read lijntje hoog is
	-- en juiste adres is meegegeven.
	
BEGIN

	PROCESS
	BEGIN
		WAIT UNTIL clock'event AND clock = '1';
		IF resetn = '0' THEN
			leds <= (others => '0');
			data <= (others => '0');
		ELSE
			IF address = '1' THEN
				data <= (others => '0');
			ELSE
				data <= (others => '1');
			END IF;
			IF NOT (prev_buttons = buttons) THEN
				prev_buttons <= buttons;
				irq <= '1';
			END IF;
			IF read = '1' THEN
				irq <= '0';
			END IF;
			leds(0) <= buttons(0);
			leds(1) <= buttons(1);
		END IF;
	END PROCESS;
END behavior;
