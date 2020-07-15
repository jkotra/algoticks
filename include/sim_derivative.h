/* Declarations */
algoticks_simresult run_sim_w_derivative(algoticks_settings settings, algoticks_config config);
algoticks_positionresult take_position_w_derivative(algoticks_signal signal, FILE *index_f, FILE *derivative_f, algoticks_settings settings, algoticks_config config, algoticks_row lastrow);