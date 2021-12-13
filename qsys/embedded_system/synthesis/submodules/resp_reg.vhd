LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all; 

ENTITY resp_reg IS
	PORT ( 	clock, resetn, address 	: IN STD_LOGIC;
			data 								: OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
			read								: IN STD_LOGIC;
			irq								: BUFFER STD_LOGIC;
			buttons 							: IN STD_LOGIC_VECTOR(1 DOWNTO 0)
	);
END resp_reg;

ARCHITECTURE behavior of resp_reg IS

	SIGNAL resp_time		: INTEGER RANGE 0 TO 2000;
	SIGNAL state 			: INTEGER RANGE 0 TO 12;		-- 10 leds, idle and finished.
	SIGNAL interrupt		: STD_LOGIC;
	
	COMPONENT reaction_meter IS
		PORT (clock, resetn					: IN STD_LOGIC;
				resp_time						: OUT INTEGER RANGE 0 TO 2000;
				state 							: OUT INTEGER RANGE 0 TO 12;
				irq								: BUFFER STD_LOGIC;
				buttons 							: IN STD_LOGIC_VECTOR(1 DOWNTO 0)
		);
	END COMPONENT reaction_meter;
	
BEGIN

	meter: reaction_meter PORT MAP (clock, resetn, resp_time, state, interrupt, buttons);

	PROCESS
	BEGIN
		WAIT UNTIL clock'event AND clock = '1';
		IF resetn = '0' THEN
			data 	<= (others => '0');
			irq 	<= '0';
		ELSE

			IF address = '1' THEN
				data <= std_logic_vector(to_unsigned(resp_time, data'length));
			ELSIF address = '0' THEN
				data <= std_logic_vector(to_unsigned(state, data'length));
			END IF;
			
			IF read = '1' THEN
				irq <= '0';
			ELSIF interrupt = '1' and irq = '0' THEN
				irq <= '1';
			END IF;
			
		END IF;
	END PROCESS;
END behavior;
