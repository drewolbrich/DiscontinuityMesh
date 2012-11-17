// Copyright 2008 Retroactive Fiasco.

#ifndef RFM_DISCMESH__REGION__INCLUDED
#define RFM_DISCMESH__REGION__INCLUDED

#include <vector>

#include <mesh/Types.h>
#include <meshretri/EndpointIdentifier.h>

class WedgeIntersector;

// Region
//
// A horizontal, possibly multisegment 1D horizontal illuminated region in wedge space.

class Region
{
public:
    Region();
    Region(float minS, float maxS, const meshretri::EndpointIdentifier &minId, 
        const meshretri::EndpointIdentifier &maxId, bool minIsD0, bool maxIsD0);
    ~Region();

    // Returns true if the region is empty.
    bool empty() const;

    // Returns true if the wedge's horizontal region is fully illuminated.
    bool full() const;

    // Returns true if the region only consists of a single span.
    bool isSingleSpan() const;

    // Returns the minimum value of the region.
    // Results are undefined if the region is empty.
    float minS() const;

    // Returns the maximum value of the region.
    // Results are undefined if the region is empty.
    float maxS() const;

    // Boolean operations on the region.
    void computeUnion(const Region &rhs);
    void computeDifference(const Region &rhs, WedgeIntersector *wedgeIntersector);

    struct Span {
        Span() : mMinS(0.0), mMaxS(0.0), mMinId(), mMaxId(),
                 mMinIsD0(false), mMaxIsD0(false) {} 
        Span(float minS, float maxS, 
            const meshretri::EndpointIdentifier &minId, 
            const meshretri::EndpointIdentifier &maxId,
            bool minIsD0, bool maxIsD0) 
            : mMinS(minS), mMaxS(maxS), mMinId(minId), mMaxId(maxId),
              mMinIsD0(minIsD0), mMaxIsD0(maxIsD0) {}
        bool operator==(const Span &rhs) const { 
            return mMinS == rhs.mMinS && mMaxS == rhs.mMaxS
                && mMinId == rhs.mMinId && mMaxId == rhs.mMaxId
                && mMinIsD0 == rhs.mMinIsD0 && mMaxIsD0 == rhs.mMaxIsD0;
        }
        float mMinS;
        float mMaxS;
        meshretri::EndpointIdentifier mMinId;
        meshretri::EndpointIdentifier mMaxId;
        bool mMinIsD0;
        bool mMaxIsD0;
    };

    // Adds a span to the region. Must not overlap with
    // existing spans, and must be to the right of existing spans.
    void appendSpan(const Span &span);

    // Return the spans making up the region.
    size_t spanCount() const;
    Span span(size_t index) const;

private:
    meshretri::EndpointIdentifier createProjectedDifferenceEndpointIdentifier(
        const meshretri::EndpointIdentifier &oldEndpointIdentifier, 
        WedgeIntersector *wedgeIntersector);

    typedef std::vector<Span> SpanVector;
    SpanVector mSpanVector;
};

#endif // RFM_DISCMESH__REGION__INCLUDED
