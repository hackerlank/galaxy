#pragma once
#pragma warning(once:4309)

namespace cc
{
	const static char key[30] = {	0xbc, 0x23, 0x9c, 0x76, 0x99,
		0xfc, 0x87, 0xe6, 0x8f, 0x7e,
		0x66, 0x1a, 0xd5, 0x9a, 0x4f,
		0xf1, 0xcd, 0x12, 0x65, 0x61,
		0xcc, 0x4e, 0x49, 0x0e, 0x11,
		0xe1, 0x34, 0x7b, 0x00, 0x0f
	};
	const static int key_len = 30;
	inline static char pick_key(int seek)
	{
		seek = (seek * 29 + 37) % key_len;
		return key[seek];
	}
	inline static void convert_binary(char* binary, int len)
	{
		if (len <= 0) return;
		int initial_key = 17;
		for (int i = 0; i < len; ++i)
		{
			(*binary) ^= pick_key(initial_key);
			binary += sizeof(char);
			initial_key = (initial_key * 27 + 19);
			if (initial_key > 3000000)
				initial_key /= 100;
		}
	}
}


