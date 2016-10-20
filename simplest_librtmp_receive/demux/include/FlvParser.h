#ifndef FLVPARSER_H
#define FLVPARSER_H

#include <vector>

#include "Videojj.h"

using namespace std;

typedef unsigned long long uint64_t;

class CFlvParser
{
public:
	CFlvParser();
	virtual ~CFlvParser();

	int Parse(unsigned char *pBuf, int nBufSize, int &nUsedLen);
	int PrintInfo();
	int DumpH264(const std::string &path);
	int DumpAAC(const std::string &path);

private:
	typedef struct FlvHeader_s
	{
		int nVersion;
		int bHaveVideo, bHaveAudio;
		int nHeadSize;

		unsigned char *pFlvHeader;
	} FlvHeader;
	typedef struct TagHeader_s
	{
		int nType;
		int nDataSize;
		int nTimeStamp;
		int nTSEx;
		int nStreamID;

		unsigned int nTotalTS;
	} TagHeader;

	class Tag
	{
	public:
		void Init(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen);

		TagHeader _header;
		unsigned char *_pTagData;
		unsigned char *_pMedia;
		int _nMediaLen;
	};

	class CVideoTag : public Tag
	{
	public:
		CVideoTag(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen, CFlvParser *pParser);

		int _nFrameType;
		int _nCodecID;
		int ParseH264Tag(CFlvParser *pParser);
		int ParseH264Configuration(CFlvParser *pParser, unsigned char *pTagData);
		int ParseNalu(CFlvParser *pParser, unsigned char *pTagData);
	};

	class CAudioTag : public Tag
	{
	public:
		CAudioTag(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen, CFlvParser *pParser);

		int _nSoundFormat;
		int _nSoundRate;
		int _nSoundSize;
		int _nSoundType;

		// aac
		static int _aacProfile;
		static int _sampleRateIndex;
		static int _channelConfig;

		int ParseAACTag(CFlvParser *pParser);
		int ParseAudioSpecificConfig(CFlvParser *pParser, unsigned char *pTagData);
		int ParseRawAAC(CFlvParser *pParser, unsigned char *pTagData);
	};

	typedef struct FlvStat_s
	{
		int nMetaNum, nVideoNum, nAudioNum;
		int nMaxTimeStamp;
		int nLengthSize = 0;
	} FlvStat;

	static unsigned int ShowU32(unsigned char *pBuf) { return (pBuf[0] << 24) + (pBuf[1] << 16) + (pBuf[2] << 8) + pBuf[3]; }
	static unsigned int ShowU24(unsigned char *pBuf) { return (pBuf[0] << 16) + (pBuf[1] << 8) + (pBuf[2]); }
	static unsigned int ShowU16(unsigned char *pBuf) { return (pBuf[0] << 8) + (pBuf[1]); }
	static unsigned int ShowU8(unsigned char *pBuf) { return (pBuf[0]); }
	static void WriteU64(uint64_t & x, int length, int value)
	{
		uint64_t mask = 0xFFFFFFFFFFFFFFFF >> (64 - length);
		x = (x << length) | ((uint64_t)value & mask);
	}

	friend Tag;
	
private:

	FlvHeader *CreateFlvHeader(unsigned char *pBuf);
	int DestroyFlvHeader(FlvHeader *pHeader);
	Tag *CreateTag(unsigned char *pBuf, int nLeftLen);
	int DestroyTag(Tag *pTag);
	int Stat();
	int StatVideo(Tag *pTag);
	int IsUserDataTag(Tag *pTag);

private:

	FlvHeader* _pFlvHeader;
	vector<Tag *> _vpTag;
	FlvStat _sStat;
	CVideojj *_vjj;

	// H.264
	int _nNalUnitLength;

};

#endif // FLVPARSER_H
