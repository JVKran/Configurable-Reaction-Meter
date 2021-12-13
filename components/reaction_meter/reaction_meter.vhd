LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY reaction_meter IS
	PORT (clock, resetn					: IN STD_LOGIC;
			resp_time						: OUT INTEGER RANGE 0 TO 2000;
			state 							: OUT INTEGER RANGE 0 TO 12;
			irq								: BUFFER STD_LOGIC;
			buttons 							: IN STD_LOGIC_VECTOR(1 DOWNTO 0)
	);
END reaction_meter;

ARCHITECTURE behavior of reaction_meter IS

	SIGNAL prev_buttons 	: STD_LOGIC_VECTOR(1 DOWNTO 0);
	
BEGIN

	PROCESS
	
		VARIABLE tick						: INTEGER RANGE 0 TO 60_000_000 := 0;
		VARIABLE time						: INTEGER RANGE 0 TO 2000;
	
	BEGIN
		WAIT UNTIL clock'event AND clock = '1';
		IF resetn = '0' THEN
			resp_time <= 0;
			time := 0;
		ELSE
			tick := tick + 1;
			IF tick >= 50_000_000 THEN
				tick := 0;
				time := time + 1;
				resp_time <= time;
				irq <= '1';
			ELSE
				irq <= '0';
			END IF;
			
		END IF;
	END PROCESS;
END behavior;
