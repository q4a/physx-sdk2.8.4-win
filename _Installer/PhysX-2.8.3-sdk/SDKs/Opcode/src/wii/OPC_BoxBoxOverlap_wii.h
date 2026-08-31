#ifdef __PPCGEKKO__
//! A special version for 2 axis-aligned boxes
inline_ BOOL AABBCollider::AABBAABBOverlap(const Point& extents, const Point& center)
{
	// Stats
	
x0:	mNbVolumeBVTests++;
	
	register const Point* p1 = &(mBox.mCenter);
	register const Point* p2 = &center;
	register const Point* p3 = &extents;
	register const Point* p4 = &(mBox.mExtents);
	register __vec2x32float__ a, b, c;
	register u32 res;

	asm volatile
	{
		psq_l a, 0(p1), 0, 0;
		psq_l b, 0(p2), 0, 0; 
		ps_sub c, a, b;
		psq_l a, 0(p3), 0, 0;
		psq_l b, 0(p4), 0, 0; 
		ps_abs c, c;		
		ps_add b, a, b;
		nop;
		ps_cmpo0 cr0, c, b;
		ble x1;
		li res,0;
		b end;
x1:		ps_cmpo1 cr0, c, b;
		ble x2;
		li res,0;
		b end;
x2:		psq_l a, 8(p1), 0, 0;
		psq_l b, 8(p2), 0, 0; 
		ps_sub c, a, b;
		psq_l a, 8(p3), 0, 0;
		psq_l b, 8(p4), 0, 0; 
		ps_abs c, c;		
		ps_add b, a, b;
		ps_cmpo0 cr0, c, b;		
		mfcr r0
		extrwi r0,r0,1,1
		cntlzw r0,r0
		srwi res,r0,5
	}
end:	return res;
}
#endif
