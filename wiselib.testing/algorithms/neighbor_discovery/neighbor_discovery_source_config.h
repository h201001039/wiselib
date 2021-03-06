//#define CONFIG_NEIGHBOR_DISCOVERY_H_RAND_STARTUP
#define CONFIG_NEIGHBOR_DISCOVERY_H_LQI_FILTERING
#define CONFIG_NEIGHBOR_DISCOVERY_H_RSSI_FILTERING
#define CONFIG_NEIGHBOR_DISCOVERY_H_TRUST_FILTERING
#define CONFIG_NEIGHBOR_DISCOVERY_H_ACTIVE_CONNECTIVITY_FILTERING
#define CONFIG_NEIGHBOR_DISCOVERY_H_ACTIVE_SCLD
#define CONFIG_NEIGHBOR_DISCOVERY_H_TRANSMISSION_POWER_PIGGY
#define CONFIG_NEIGHBOR_DISCOVERY_H_SMALL_PAYLOAD
#ifdef CONFIG_NEIGHBOR_DISCOVERY_H_SMALL_PAYLOAD
	#define CONFIG_NEIGHBOR_DISCOVERY_H_SMALL_PAYLOAD_LINK_STAB
	//#define CONFIG_NEIGHBOR_DISCOVERY_H_SMALL_PAYLOAD_LQI
	//#define CONFIG_NEIGHBOR_DISCOVERY_H_SMALL_PAYLOAD_RSSI
	#define CONFIG_NEIGHBOR_DISCOVERY_H_SMALL_PAYLOAD_TRUST
#endif
#define CONFIG_NEIGHBOR_DISCOVER_H_RAND_BACKOFF_BEACONS

#define CONFIG_NEIGHBOR_DISCOVERY_H_COORD_SUPPORT
#ifdef CONFIG_NEIGHBOR_DISCOVERY_H_COORD_SUPPORT
#define UNIGE_TESTBED
//#define CTI_TESTBED
#define CONFIG_NEIGHBOR_DISCOVERY_H_COORD_SUPPORT_2D
//#define CONFIG_NEIGHBOR_DISCOVERY_H_COORD_SUPPORT_3D
//#define CONFIG_NEIGHBOR_DISCOVERY_H_COORD_SUPPORT_SHAWN
#define CONFIG_NEIGHBOR_DISCOVERY_H_COORD_SUPPORT_ISENSE
#ifdef CONFIG_NEIGHBOR_DISCOVERY_H_COORD_SUPPORT_SHAWN
typedef double PositionNumber;
#endif
#ifdef CONFIG_NEIGHBOR_DISCOVERY_H_COORD_SUPPORT_ISENSE
typedef uint8_t PositionNumber;
#endif
#endif


#define DEBUG_NEIGHBOR_DISCOVERY
#ifdef DEBUG_NEIGHBOR_DISCOVERY
	#define DEBUG_NEIGHBOR_H
//	#define DEBUG_PROTOCOL_PAYLOAD_H
//	#define DEBUG_BEACON_H
//	#define DEBUG_PROTOCOL_SETTINGS_H
	#define DEBUG_PROTOCOL_H
	#define DEBUG_NEIGHBOR_DISCOVERY_STATS
//	#define DEBUG_NEIGHBOR_DISCOVERY_STATS_DAEMON
//	#define DEBUG_NEIGHBOR_DISCOVERY_H
//	#ifdef DEBUG_NEIGHBOR_DISCOVERY_H
//	#define DEBUG_NEIGHBOR_DISCOVERY_H_BEACON_SCHEDULER
//		#define DEBUG_NEIGHBOR_DISCOVERY_H_BEACONS
//		#define DEBUG_NEIGHBOR_DISCOVERY_H_RECEIVE
//		#define DEBUG_NEIGHBOR_DISCOVERY_H_REGISTER_PROTOCOL
//		#define DEBUG_NEIGHBOR_DISCOVERY_H_ND_DAEMON
//		#define DEBUG_NEIGHBOR_DISCOVERY_H_NEIGHBOR_UPDATE_LINK_STAB_RATIO
//		#define DEBUG_NEIGHBOR_DISCOVERY_H_ND_METRICS_DAEMON
//	#endif
#endif
