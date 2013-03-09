static int
panel_parse_global(Panel *p, char **fp)
{
    cfg_tree_t cfg = cfg_init();
    
    // this is global panel config
    {
        cfg_tree_t gl = cfg_tree_sub( cfg, "Global" );
        /* cfg_get_enum/bool/int routines are synonims */
        cfg_get_enum( gl, "edge", &p->edge );
        cfg_get_enum( gl, "allign", &p->allign );
        cfg_get_int( gl, "margin", &p->margin );
        cfg_get_enum( gl, "widthtype", &p->widthtype );
        cfg_get_int( gl, "width", &p->width );
        cfg_get_enum( gl, "heighttype", &p->heighttype );
        cfg_get_int( gl, "height", &p->height );
        cfg_get_int( gl, "spacing", &p->spacing );
        cfg_get_bool( gl, "SetDockType", &p->setdocktype );
        cfg_get_bool( gl, "SetPartialStrut", &p->setstrut );
        cfg_get_bool( gl, "RoundCorners", &p->round_corners );
        cfg_get_bool( gl, "Transparent", &p->transparent );
        cfg_get_int( gl, "Alpha", &p->alpha );
        cfg_get_bool( gl, "AutoHide", &p->autohide );
        cfg_get_int( gl, "HeightWhenHidden", &p->height_when_hidden );
        cfg_get_int( gl, "spacing", &p->spacing );
        /* blob will be transformed to right endian and put into destination */
        cfg_get_blob( gl, "TintColor", &p->tintcolor, COLOR_SIZE );
        cfg_get_bool( gl, "useFontColor", &p->usefontcolor );
        cfg_get_blob( gl, "FontColor", &p->fontcolor, COLOR_SIZE );
        cfg_get_bool( gl, "useFontSize", &p->usefontsize );
        cfg_get_int( gl, "FontSize", &p->fontsize );
        cfg_get_bool( gl, "Background", &p->background );
        cfg_get_cstr( gl, "BackgroundFile", &p->background_file );
        cfg_get_int( gl, "IconSize", &p->iconsize );
        if (!log_level_set_on_commandline)
            cfg_get_int( gl, "LogLevel", &log_level );
    }

    {
        cfg_tree_t plugins = cfg_tree_sub( cfg, 'Plugins' );
        char *plugname;
        Plugin *plug = NULL;
        for( cfg_tree_iter_t i = cfg_iteration_start( plugins );
            i != NULL;
            i = cfg_iteration_next( i )
        ) {
            cfg_get_cstr( i.child, "type", &plugname );

            if (!plugname || !(plug = plugin_load(plugname))) {
                ERR( "lxpanel: can't load %s plugin\n", plugname);
                goto error;
            }

            plug->panel = p;
            if (plug->class->expand_available)
                 cfg_get_int( i.child, "expand", &plug->expand );
            cfg_get_int( i.child, "padding", &plug->padding )
            cfg_get_int( i.child, "border", &plug->border );

            DBG("starting\n");

            if (!plugin_start(plug, cfg_tree_sub( i.child, "Config" ) ) ) {
                ERR( "lxpanel: can't start plugin %s\n", type);
                goto error;
            }

            DBG("plug %s\n", type);
            p->plugins = g_list_append(p->plugins, plug);
        }

      error:
        if (plug != NULL)
            plugin_unload(plug);
        free( plugname );
        cfg_destroy( cfg );
        RET(0);
    }

    cfg_destroy( cfg );

    panel_normalize_configuration(p);

    return 1;
}