<game_object_definition>
		<track_info name="track_list" >
			<tracks value="forest1 forest2" />
		</track_info>
		
		<vehicle_info name="vehicle_list" >
			<vehicles value="carrera nimrud evo" />
		</vehicle_info>

		<track name="forest1" >
			<file_name value="track1.scene" />
			<preview value="track_1.png" />
			<nice_name value="Tour Along Trees" />
			<lap_number value="4" />
			<sky_dome value="" />
			<sky_box value="CloudyNoonSkyBox" />
			<needs_additional_light value="true" />
			<difficulty value="61" />
			<scene_ambient value="0.76 0.76 0.76" />
		</track>
		
		<track name="forest2" >
			<file_name value="track2.scene" />
			<preview value="track_2.png" />
			<nice_name value="Long Way Ahead" />
			<lap_number value="2" />
			<needs_additional_light value="true" />
			<sky_box value="EarlyMorningSkyBox" />
			<sky_dome value="" />
			<difficulty value="76" />
			<scene_ambient value="0.12 0.45 0.12" />
		</track>

		<game_object name="default_static" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="CONVEX" />
                <animated value="false" />
        </game_object>
		
		<game_object name="rail" >
				<mass value="0" />
                <dynamic value="false" />
                <collision-shape value="MESH" />
                <animated value="false" />
		</game_object>
       
        <game_object name="road" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="MESH" />
                <animated value="false" />
        </game_object>
		
		<game_object name="roadblock" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="CONVEX" />
                <animated value="false" />
        </game_object>
		
		<game_object name="lane" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="CONVEX" />
                <animated value="false" />
        </game_object>
		
		<game_object name="tree" >
				<mass value="0" />
                <dynamic value="false" />
                <collision-shape value="CONVEX" />
                <animated value="false" />
		</game_object>
		
		<game_object name="checkpoint" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="BOX" />
                <animated value="false" />
				<collisionResponse value="false" />
        </game_object>
		
		<game_object name="barrier" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="CONVEX" />
                <animated value="false" />
        </game_object>
		
		<game_object name="fence" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="CONVEX" />
                <animated value="false" />
        </game_object>
		
		<game_object name="tarmac" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="CONVEX" />
                <animated value="false" />
        </game_object>
		
		<game_object name="grass" >
                <mass value="0" />
                <dynamic value="false" />
                <collision-shape value="CONVEX" />
                <animated value="false" />
        </game_object>

		<game_object name="nimrud" >
				<meshFile value="nimrud_chassis.mesh" />
				<wheels value="nimrudFL nimrudFR nimrudBL nimrudBR" />
                <mass value="700" />
                <dynamic value="true" />
                <collision-shape value="CONVEX" />
				<gauge_type value="hud/gauge3" /> <!-- material name of gauge -->
                <animated value="false" />
				<vehicle_preview value="nimrud_preview.png" />
				<nice_name value="Nimrud" />
				
				<engineProgressValue value="62"/>
				<maxSpeedProgressValue value="65"/>
				<accelProgressValue value="56"/>
				<tractionProgressValue value="81"/>
				
				<wheelFriction value="5.6" />
				<skidInfo value="0.2" />
				
				<suspensionRestLength value="0.50" />
				<suspensionStiffness value="50.0" />
				<suspensionDamping value="7.07" />
				<suspensionCompression value="4.24" />
				
				<rollInfluence value="0.001" />
				
				<engineForce value="500" />
				<maxEngineForce value="2000" />
				<speedGainIdeal value="150" />
				<maxReverseSpeed value="30" />
				
				<startUpBoost value="5000" />
				<startUpBoostLimit value="75" />
				
				<brakeForce value="150" />
				<brakeGainIdeal value="100" />
				<brakeReductionSpeed value="100" />
				<brakeGainReduced value="50" />
				
				<steeringIncrementIdeal value="0.025" /> <!-- Steering value when speed is under reduction speed -->
				<steeringIncrementReduced value="0.019" /> <!-- Reduced steering value -->
				<steeringReductionSpeed value="100" /> <!-- The speed above which steering is adjusted -->
				<steeringClamp value="0.5" /> <!-- Max steering turn -->
				
				<engineSound value="engine.ogg" />
				<brakeSound value="braking.wav" />
        </game_object>
		
		<game_object name="nimrudBR" >
                <radius value="0.60" />
				<isFront value="false" />
				<connectionPoint value="1.31 0.0 -2.702" />
				<meshFile value="nimrudBR.mesh" />
        </game_object>
		
		<game_object name="nimrudBL" >
                <radius value="0.60" />
				<isFront value="false" />
				<connectionPoint value="-1.31 0.0 -2.6702" />
				<meshFile value="nimrudBL.mesh" />
        </game_object>
		
		<game_object name="nimrudFR" >
                <radius value="0.60" />
				<isFront value="true" />
				<connectionPoint value="1.31 0.0 2.389" />
				<meshFile value="nimrudFR.mesh" />
        </game_object>
		
		<game_object name="nimrudFL" >
                <radius value="0.60" />
				<isFront value="true" />
				<meshFile value="nimrudFL.mesh" />
				<connectionPoint value="-1.31 0.0 2.389" />
        </game_object>
		
		<game_object name="carrera" >
				<meshFile value="carrera_chassis.mesh" />
				<wheels value="carreraFL carreraFR carreraBL carreraBR" />
                <mass value="825" />
                <dynamic value="true" />
                <collision-shape value="CONVEX" />
				<gauge_type value="hud/gauge4" />
				<vehicle_preview value="carrera_preview.png" />
				<nice_name value="Porsche Carrera" />
                <animated value="false" />
				
				<engineProgressValue value="76"/>
				<maxSpeedProgressValue value="88"/>
				<accelProgressValue value="76"/>
				<tractionProgressValue value="65"/>
				
				<wheelFriction value="5.13" />
				<skidInfo value="0.218" />
				
				<suspensionRestLength value="0.50" />
				<suspensionStiffness value="80.0" />
				<suspensionDamping value="4.07" />
				<suspensionCompression value="3.57" />
				
				<rollInfluence value="0.001" />
				
				<engineForce value="935" />
				<maxEngineForce value="2630" />
				<speedGainIdeal value="150" />
				<maxReverseSpeed value="45" />
				
				<startUpBoost value="5300" />
				<startUpBoostLimit value="95" />
				
				<brakeForce value="175" />
				<brakeGainIdeal value="100" />
				<brakeReductionSpeed value="100" />
				<brakeGainReduced value="50" />
				
				<steeringIncrementIdeal value="0.035" /> <!-- Steering value when speed is under reduction speed -->
				<steeringIncrementReduced value="0.017" /> <!-- Reduced steering value -->
				<steeringReductionSpeed value="110" /> <!-- The speed above which steering is adjusted -->
				<steeringClamp value="0.6" /> <!-- Max steering turn -->
				
				<engineSound value="carrera_engine.wav" />
				<brakeSound value="braking.wav" />
        </game_object>
		
		<game_object name="carreraBR" >
                <radius value="0.70" />
				<isFront value="false" />
				<connectionPoint value="1.88 0.0 -3.152" />
				<meshFile value="carrera_wheelBR.mesh" />
        </game_object>
		
		<game_object name="carreraBL" >
                <radius value="0.70" />
				<isFront value="false" />
				<connectionPoint value="-1.78 0.0 -3.152" />
				<meshFile value="carrera_wheelBL.mesh" />
        </game_object>
		
		<game_object name="carreraFR" >
                <radius value="0.63" />
				<isFront value="true" />
				<connectionPoint value="1.88 0.0 2.985" />
				<meshFile value="carrera_wheelFR.mesh" />
        </game_object>
		
		<game_object name="carreraFL" >
                <radius value="0.63" />
				<isFront value="true" />
				<meshFile value="carrera_wheelFL.mesh" />
				<connectionPoint value="-1.78 0.0 2.985" />
        </game_object>
		
		<game_object name="evo" >
				<meshFile value="evo_chassis.mesh" />
				<wheels value="evoFL evoFR evoBL evoBR" />
                <mass value="700" />
                <dynamic value="true" />
                <collision-shape value="CONVEX" />
				<gauge_type value="hud/gauge4" /> <!-- material name of gauge -->
                <animated value="false" />
				<vehicle_preview value="evo_preview.png" />
				<nice_name value="Mitsubishi Evo" />
				
				<engineProgressValue value="68"/>
				<maxSpeedProgressValue value="75"/>
				<accelProgressValue value="65"/>
				<tractionProgressValue value="76"/>
				
				<wheelFriction value="5.1" />
				<skidInfo value="0.1" />
				
				<suspensionRestLength value="0.50" />
				<suspensionStiffness value="49.0" />
				<suspensionDamping value="6.88" />
				<suspensionCompression value="4.21" />
				
				<rollInfluence value="0.001" />
				
				<engineForce value="850" />
				<maxEngineForce value="2350" />
				<speedGainIdeal value="160" />
				<maxReverseSpeed value="35" />
				
				<startUpBoost value="6500" />
				<startUpBoostLimit value="100" />
				
				<brakeForce value="150" />
				<brakeGainIdeal value="100" />
				<brakeReductionSpeed value="100" />
				<brakeGainReduced value="50" />
				
				<steeringIncrementIdeal value="0.025" /> <!-- Steering value when speed is under reduction speed -->
				<steeringIncrementReduced value="0.019" /> <!-- Reduced steering value -->
				<steeringReductionSpeed value="100" /> <!-- The speed above which steering is adjusted -->
				<steeringClamp value="0.5" /> <!-- Max steering turn -->
				
				<engineSound value="evo_engine.wav" />
				<brakeSound value="braking.wav" />
        </game_object>
		
		<game_object name="evoBL" >
                <radius value="0.6" />
				<isFront value="false" />
				<connectionPoint value="1.42 0.0 -2.702" />
				<meshFile value="evo_BL.mesh" />
        </game_object>
		
		<game_object name="evoBR" >
                <radius value="0.6" />
				<isFront value="false" />
				<connectionPoint value="-1.475 0.0 -2.702" />
				<meshFile value="evo_BR.mesh" />
        </game_object>
		
		<game_object name="evoFL" >
                <radius value="0.6" />
				<isFront value="true" />
				<connectionPoint value="1.42 0.0 2.639" />
				<meshFile value="evo_FL.mesh" />
        </game_object>
		
		<game_object name="evoFR" >
                <radius value="0.6" />
				<isFront value="true" />
				<meshFile value="evo_FR.mesh" />
				<connectionPoint value="-1.475 0.0 2.639" />
        </game_object>
		
</game_object_definition>