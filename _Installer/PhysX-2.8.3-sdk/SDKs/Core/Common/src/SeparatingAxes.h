// SUPPORT FILE FROM ICE, NEEDED FOR CONVEX-CONVEX - DO NOT TOUCH !

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICESEPARATINGAXES_H
#define ICESEPARATINGAXES_H

	// ### TODO better later

	// This class holds a list of potential separating axes.
	// - the orientation is irrelevant so V and -V should be the same vector
	// - the scale is irrelevant so V and n*V should be the same vector
	// - a given separating axis should appear only once in the class
	class SeparatingAxes
	{
		public:
		inline_						SeparatingAxes()	{}
		inline_						~SeparatingAxes()	{}

		bool AddAxis(const Point& axis);

		inline_	bool				AddAxis(const Point& axis, udword index)
									{
										bool b = AddAxis(axis);
										if(b)	mIndices.Add(index);
										return b;
									}

		inline_	const Vertices&		GetAxes()		const	{ return mAxes;		}
		inline_	const Container&	GetIndices()	const	{ return mIndices;	}

		inline_	void				Reset()
									{
									mAxes.Reset();
									mIndices.Reset();
									}
		private:
		Vertices	mAxes;
				Container			mIndices;
	};

#endif	// ICESEPARATINGAXES_H

