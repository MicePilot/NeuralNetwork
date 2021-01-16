#ifndef MACRO_H
#define MACRO_H


#define BAREMACHINE 1
//定义对地址原子操作的宏，适用于unsigned int指针的原子操作
#ifdef BAREMACHINE
	#define MAT_XADD(addr, delta) ((*(unsigned int volatile*)addr)=(*(unsigned int volatile*)addr)+delta)
#elif _WIN32	
	#define MAT_XADD(addr, delta) (int)_InterlockedExchangeAdd((long volatile*)addr, delta)
#endif // OS


//flag 0-2 bit:数据深度 也是就是数据类型 000 MAT_8U
//						    			 001 MAT_8S
//										 011 MAT_16U
//										 100 MAT_16S
//										 101 MAT_32S
//										 110 MAT_32F
//										 111 MAT_64F
//	   3-11 bit:通道数 最高512

#define MAT_CN_MAX 512
#define MAT_CN_SHIFT 3      //由于深度是由三个位表示的，这里设置为3即100

#define MAT_DEPTH_MAX  (1 << MAT_CN_SHIFT)	//左移三位，最大位深标识加一
#define MAT_DEPTH_MASK       (MAT_DEPTH_MAX - 1)
#define MAT_DEPTH(flags)     ((flags) & MAT_DEPTH_MASK)  //根据flag测试位深，即获得数据类型

#define MAT_MAKETYPE(depth,cn) (MAT_DEPTH(depth) + (((cn)-1) << MAT_CN_SHIFT))//制造带通道的数据类型

#define MAT_CHANNELS_MASK ((MAT_CN_MAX-1) << MAT_CN_SHIFT)
#define MAT_CHANNELS(flags)	 ((((flags) & MAT_CHANNELS_MASK )>> MAT_CN_SHIFT)+1)	//根据flag测试通道，这里直接获得通道具体值

#define MAT_TYPE_MASK        (MAT_DEPTH_MAX*MAT_CN_MAX - 1)				
#define MAT_TYPE(flags)      ((flags) & MAT_TYPE_MASK)			//类型测试，包含通道和数据类型

#define MAT_8UC1 MAT_MAKETYPE(MAT_8U,1)
#define MAT_8UC2 MAT_MAKETYPE(MAT_8U,2)
#define MAT_8UC3 MAT_MAKETYPE(MAT_8U,3)
#define MAT_8UC4 MAT_MAKETYPE(MAT_8U,4)
#define MAT_8UC(n) MAT_MAKETYPE(MAT_8U,(n))

#define MAT_8SC1 MAT_MAKETYPE(MAT_8S,1)
#define MAT_8SC2 MAT_MAKETYPE(MAT_8S,2)
#define MAT_8SC3 MAT_MAKETYPE(MAT_8S,3)
#define MAT_8SC4 MAT_MAKETYPE(MAT_8S,4)
#define MAT_8SC(n) MAT_MAKETYPE(MAT_8S,(n))

#define MAT_16UC1 MAT_MAKETYPE(MAT_16U,1)
#define MAT_16UC2 MAT_MAKETYPE(MAT_16U,2)
#define MAT_16UC3 MAT_MAKETYPE(MAT_16U,3)
#define MAT_16UC4 MAT_MAKETYPE(MAT_16U,4)
#define MAT_16UC(n) MAT_MAKETYPE(MAT_16U,(n))

#define MAT_16SC1 MAT_MAKETYPE(MAT_16S,1)
#define MAT_16SC2 MAT_MAKETYPE(MAT_16S,2)
#define MAT_16SC3 MAT_MAKETYPE(MAT_16S,3)
#define MAT_16SC4 MAT_MAKETYPE(MAT_16S,4)
#define MAT_16SC(n) MAT_MAKETYPE(MAT_16S,(n))

#define MAT_32SC1 MAT_MAKETYPE(MAT_32S,1)
#define MAT_32SC2 MAT_MAKETYPE(MAT_32S,2)
#define MAT_32SC3 MAT_MAKETYPE(MAT_32S,3)
#define MAT_32SC4 MAT_MAKETYPE(MAT_32S,4)
#define MAT_32SC(n) MAT_MAKETYPE(MAT_32S,(n))

#define MAT_32FC1 MAT_MAKETYPE(MAT_32F,1)
#define MAT_32FC2 MAT_MAKETYPE(MAT_32F,2)
#define MAT_32FC3 MAT_MAKETYPE(MAT_32F,3)
#define MAT_32FC4 MAT_MAKETYPE(MAT_32F,4)
#define MAT_32FC(n) MAT_MAKETYPE(MAT_32F,(n))

#define MAT_64FC1 MAT_MAKETYPE(MAT_64F,1)
#define MAT_64FC2 MAT_MAKETYPE(MAT_64F,2)
#define MAT_64FC3 MAT_MAKETYPE(MAT_64F,3)
#define MAT_64FC4 MAT_MAKETYPE(MAT_64F,4)
#define MAT_64FC(n) MAT_MAKETYPE(MAT_64F,(n))

enum  MatType
{
	MAT_8U, MAT_8S, MAT_16U, MAT_16S, MAT_32S, MAT_32F, MAT_64F
};

#endif // !MACRO_H

