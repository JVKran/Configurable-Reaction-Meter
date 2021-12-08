LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY seven_segment_controller IS
	PORT ( clock, resetn, address : IN STD_LOGIC;
		write, chipselect : IN STD_LOGIC;
		writedata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
		byteenable : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
		Q_export : OUT STD_LOGIC_VECTOR(47 DOWNTO 0) 
	);
END seven_segment_controller;

ARCHITECTURE Structure OF seven_segment_controller IS
	SIGNAL local_byteenable : STD_LOGIC_VECTOR(3 DOWNTO 0);
	SIGNAL to_reg, from_reg : STD_LOGIC_VECTOR(31 DOWNTO 0);
	
	COMPONENT reg32
		PORT ( clock, resetn, address : IN STD_LOGIC;
			D : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
			byteenable : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
			Q : OUT STD_LOGIC_VECTOR(47 DOWNTO 0) 
		);
	END COMPONENT;
BEGIN
	to_reg <= writedata;
	WITH (chipselect AND write) SELECT
		local_byteenable <= byteenable WHEN '1', "0000" WHEN OTHERS;
	reg_instance: reg32 PORT MAP (clock, resetn, address, to_reg, local_byteenable, Q_export);
END Structure;