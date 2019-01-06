#include "pcapread.h"

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	static int count = 1;
	
	// declare pointer to packet header
	const struct sniff_ethernet *ethernet;	/* The ethernet header */ 
	const struct sniff_ip *ip;		/* The IP header */
	const struct sniff_tcp *tcp;		/* The TCP header */
	const char* payload;			/* Packet payload */

	int size_ip;
	int size_tcp;

	printf("\nPacket Number %d:\n", count);
	count++;

	// define ethernet header
	ethernet = (struct sniff_ethernet*)(packet);
	
	// compute ip header offset
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;

	if(size_ip < 20)
	{
		printf("	* Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	// print information
	printf("	FROM: %s\n", inet_ntoa(ip->ip_src));
	printf("	  TO: %s\n\n", inet_ntoa(ip->ip_dst));

	// determine protocal
	switch(ip->ip_p)
	{
		case IPPROTO_TCP:
			printf("	Protocol: TCP\n");
			break;
		case IPPROTO_UDP:
			printf("	Protocol: UDP\n");
			return;
		case IPPROTO_ICMP:
			printf("	Protocol: ICMP\n");
			return;
		case IPPROTO_IP:
			printf("	Protocol: IP\n");
			return;
		default:
			printf("	Protocol: unknown\n");
			return;

	}

	/* TCP case */
	tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET +size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if(size_tcp < 20)
	{
		printf("	* Invalid TCP header length: %u bytes\n", size_tcp);
		return;
	}

	printf("	Src port: %d\n", ntohs(tcp->th_sport));
	printf("	Dst port: %d\n", ntohs(tcp->th_dport));
	printf("	Packet length: %hu\n", header->len);


	// tcp payload size
	payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);

	time_t time;
	struct tm *t;
	char buffer[80];

	time = header->ts.tv_sec;
	t = localtime(&time);
	
	strftime(buffer, 80, "%x - %H:%M", t);

	printf("	Packet time: %s\n", buffer);

	return;
}






int main(int argc, char **argv)
{
	pcap_t *handle;			// Session Handle
	char *dev;			// The device name
	char errbuf[PCAP_ERRBUF_SIZE];	// Error String

	struct bpf_program filter;	// The compiled filter expression
	char filter_exp[4096];	// The filer expression
	bpf_u_int32 mask;		// The netmask of our sniffing device
	bpf_u_int32 net;		// The IP of out sniffing device
	
	int cmd_opt;			
	

	// setting the device
	dev = pcap_lookupdev(errbuf);
	if(dev == NULL)
	{
		printf("Couldn't find default device: %s\n", errbuf);
		return 1;
	}

	// get device info
	if(pcap_lookupnet(dev, &net, &mask,errbuf) == -1)
	{
		printf("Can't get netmask for device %s: %s\n", dev, errbuf);
		net = 0;
		mask = 0;
	}
	
	// print device info
	printf("Device: %s\n", dev);

	/*
	//opening the device for sniffing
	handle = pcap_open_live(dev, BUFSIZE, 0, 10000, errbuf);
	if(handle == NULL)
	{
		printf("Couldn't open device %s: %s", dev, errbuf);
	}
	*/
	
	while((cmd_opt = getopt(argc, argv, "p:f:")) != -1)
	{
		switch(cmd_opt)
		{
			case 'p':
				printf("	File: %s\n", optarg);
				handle = pcap_open_offline(optarg, errbuf);
				break;
			case 'f':
				strcat(filter_exp, optarg);
				strcat(filter_exp, " ");
				while(optind < argc)
				{
					if(argv[optind][0] == '-')
						break;
					strcat(filter_exp, argv[optind++]);
					strcat(filter_exp, " ");
				}
				printf("	Filter expression: %s\n", filter_exp);
				break;


		}
	}
	
	/*test
	
	// the problem maybe I didn't use pcap_open_offline
	handle = pcap_open_offline("ipv4frags.pcap", errbuf);
	*/

	// compoile the filter expression
	if(pcap_compile(handle, &filter, filter_exp, 0, net) == -1)
	{
		printf("Couldn't parse filter %s: %s\n",filter_exp, pcap_geterr(handle));
		return 2;
	}

	// apply the compiled filer
	if(pcap_setfilter(handle, &filter) == -1)
	{
		printf("Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return 2;
	}


	printf("Start Sniffing\n\n");

	// set our callback function
	pcap_loop(handle, 0, got_packet, NULL);
	
	// cleanup
	pcap_freecode(&filter);
	pcap_close(handle);

	printf("\nCapture Over\n");

	return 0;
}
