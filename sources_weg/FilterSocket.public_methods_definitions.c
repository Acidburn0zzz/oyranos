/** Function  oyFilterSocket_Callback
 *  @memberof oyFilterSocket_s
 *  @brief    Tell about a oyConversion_s event
 *
 *  @param[in,out] c                   the connector
 *  @param         e                   the event type
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 *  @date    2009/02/19
 */
OYAPI int  OYEXPORT
                 oyFilterSocket_Callback(
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e )
{
  int n, i;
  oyFilterSocket_s_ * s_;
  oyFilterPlug_s * p;

  oyFilterPlug_s_ ** c_ = (oyFilterPlug_s_**)&c;

  if(e != oyCONNECTOR_EVENT_OK && oy_debug_signals)
  {
    WARNc5_S("\n  oyFilterNode_s[%d]->oyFilterSocket_s[%d]\n"
             "  event: \"%s\" plug[%d/node%d]",
            (c && (*c_)->remote_socket_ && (*c_)->remote_socket_->node) ?
                   oyObject_GetId((*c_)->remote_socket_->node->oy_) : -1,
            (c && (*c_)->remote_socket_) ? oyObject_GetId((*c_)->remote_socket_->oy_)
                                     : -1,
            oyConnectorEventToText(e),
            c ? oyObject_GetId( c->oy_ ) : -1,
            c ? ((*c_)->node ? oyObject_GetId( (*c_)->node->oy_ ) : -1) : -1
          );
  }

  if(!c)
    return 1;

  s_ = (oyFilterSocket_s_*) (*c_)->remote_socket_;

  if(!s_)
    return 0;

  n = oyFilterPlugs_Count( s_->requesting_plugs_ );

  if(e == oyCONNECTOR_EVENT_RELEASED)
    for(i = 0; i < n; ++i)
    {
      p = oyFilterPlugs_Get( s_->requesting_plugs_, i );
      if(p == c)
      {
        oyFilterPlugs_ReleaseAt( s_->requesting_plugs_, i );
        break;
      }
    }

  return 0;
}

#ifdef UNHIDE_FILTERSOCKET
/** Function  oyFilterSocket_SignalToGraph
 *  @memberof oyFilterSocket_s
 *  @brief    Send a signal through the graph
 *
 *  The traversal direction is defined as from the starting node to the output.
 *
 *  @return                            1 if handled or zero
 *
 *  @version Oyranos: 0.3.2
 *  @since   2009/10/27 (Oyranos: 0.1.10)
 *  @date    2011/07/10
 */
OYAPI int  OYEXPORT
                 oyFilterSocket_SignalToGraph (
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e )
{
  oySIGNAL_e sig = oySIGNAL_OK;
  int n, i, j_n,j, k,k_n, handled = 0, pos;
  oyFilterPlug_s * p, * sp;
  oyFilterGraph_s * graph = 0;

  oyFilterSocket_s_ ** c_ = (oyFilterSocket_s_**)&c;

  switch(e)
  {
    case oyCONNECTOR_EVENT_OK:                /**< kind of ping */
    case oyCONNECTOR_EVENT_CONNECTED:         /**< connection established */
    case oyCONNECTOR_EVENT_RELEASED:          /**< released the connection */
    case oyCONNECTOR_EVENT_DATA_CHANGED:      /**< call to update image views */
    case oyCONNECTOR_EVENT_STORAGE_CHANGED:   /**< new data accessors */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_DATA: /**< can not process image */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION:/**< can not handle option */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT:/**< can not handle profile */
    case oyCONNECTOR_EVENT_INCOMPLETE_GRAPH:  /**< can not completely process */
         sig = (oySIGNAL_e) e; break;
  }

  handled = oyStruct_ObserverSignal( (oyStruct_s*) (*c_)->node, sig, 0 );
  if(handled)
    return handled;

  if(e != oyCONNECTOR_EVENT_OK && oy_debug_signals)
  {
    WARNc4_S("oyFilterNode_s[%d]->oyFilterSocket_s[%d]\n"
             "  event: \"%s\" socket[%d]",
            (c && (*c_)->node) ? oyObject_GetId((*c_)->node->oy_) : -1,
            c ? oyObject_GetId(c->oy_) : -1,
            oyConnectorEventToText(e),
            (c) ? oyObject_GetId( c->oy_ ) : -1
          );
  }

  n = oyFilterPlugs_Count( (*c_)->requesting_plugs_ );

  for(i = 0; i < n; ++i)
  {
    p = oyFilterPlugs_Get( (*c_)->requesting_plugs_, i );
    handled = oyStruct_ObserverSignal( (oyStruct_s*) oyFilterPlugPriv_m(p)->node, sig, 0 );

    /* get all nodes in the output direction */
    graph = oyFilterGraph_FromNode( oyFilterPlugPriv_m(p)->node, OY_INPUT );

    j_n = oyFilterNodes_Count( oyFilterGraphPriv_m(graph)->nodes );
    for( j = 0; j < j_n; ++j )
    {
      oyFilterNode_s * node = oyFilterNodes_Get( oyFilterGraphPriv_m(graph)->nodes, j );

      /* iterate over all node outputs */
      k_n = oyFilterNode_EdgeCount( node, 1, OY_FILTEREDGE_CONNECTED );
      for(k = 0; k < k_n; ++k)
      {
        pos = oyFilterNode_GetConnectorPos( node, 1, "///", k,
                                            OY_FILTEREDGE_CONNECTED );
        sp = oyFilterNode_GetPlug( node, pos );
        oyFilterSocket_Callback( sp, e );
      }

      oyFilterNode_Release( &node );
      if(handled)
        break;
    }

    oyFilterGraph_Release( &graph );
    if(handled)
      break;
  }

  return handled;
}
#endif /*UNHIDE_FILTERSOCKET*/

/** Function  oyFilterSocket_GetNode
 *  @memberof oyFilterSocket_s
 *  @brief    Access oyFilterSocket_s::node
 *
 *  @param[in]     socket              the socket
 *  @return                            the node
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterSocket_GetNode  ( oyFilterSocket_s  * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  return oyFilterNode_Copy( s->node, 0 );
}
/** Function  oyFilterSocket_GetPlugs
 *  @memberof oyFilterSocket_s
 *  @brief    Access oyFilterSocket_s::requesting_plugs_
 *
 *  @param[in]     socket              the socket
 *  @return                            the requesting plugs
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyFilterPlugs_s * OYEXPORT
             oyFilterSocket_GetSocket( oyFilterSocket_s  * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  return oyFilterPlugs_Copy( s->requesting_plugs_, 0 );
}
/** Function  oyFilterPlug_GetPattern
 *  @memberof oyFilterPlug_s
 *  @brief    Access oyFilterPlug_s::pattern
 *
 *  @param[in]     socket              the socket
 *  @return                            capabilities pattern for this filter socket
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyConnector_s * OYEXPORT
             oyFilterSocket_GetPattern(oyFilterSocket_s  * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  return oyConnector_Copy( s->pattern, 0 );
}
/** Function  oyFilterSocket_GetRelatives
 *  @memberof oyFilterSocket_s
 *  @brief    get short description of belonging filter
 *
 *  @param[in]     socket              the socket
 *  @return                            short text
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI const char * OYEXPORT
             oyFilterSocket_GetRelatives (
                                       oyFilterSocket_s    * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  return s->relatives_;
}
