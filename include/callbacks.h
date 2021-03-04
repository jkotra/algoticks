int load_callbacks(algoticks_config config);
int send_callbacks(algoticks_event events);
int close_callbacks();
algoticks_event make_event_from_positionresult(algoticks_positionresult positionresult);
algoticks_event make_event_from_signal(algoticks_signal signal);
algoticks_event make_event_from_position(algoticks_row pos_storage, algoticks_dashboard dashboard);
