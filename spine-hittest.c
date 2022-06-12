#include "spine-hittest.h"

typedef struct {
	float x;
	float y;
	float* ps;
	int vl;
} PIPQ;

int IsPointOnLine(float px0, float py0, float px1, float py1, float px2, float py2)
{
	int flag = 0;
	float d1 = (px1 - px0) * (py2 - py0) - (px2 - px0) * (py1 - py0);
	if ((fabs(d1) < 1.192092896e-07F) && ((px0 - px1) * (px0 - px2) <= 0) && ((py0 - py1) * (py0 - py2) <= 0))
	{
		flag = 1;
	}
	return flag;
}

int IsIntersect(float px1, float py1, float px2, float py2, float px3, float py3, float px4, float py4)
{
	int flag = 0;
	float d = (px2 - px1) * (py4 - py3) - (py2 - py1) * (px4 - px3);
	if (d != 0)
	{
		float r = ((py1 - py3) * (px4 - px3) - (px1 - px3) * (py4 - py3)) / d;
		float s = ((py1 - py3) * (px2 - px1) - (px1 - px3) * (py2 - py1)) / d;
		if ((r >= 0) && (r <= 1) && (s >= 0) && (s <= 1))
		{
			flag = 1;
		}
	}
	return flag;
}

int Point_In_Polygon_2D(PIPQ* q)
{
	/*for (int i = 0; i < q->vl; i += 2) {
		printf("(%f, %f) ", q->ps[i], q->ps[i | 1]);
	}*/
	int isInside = 0;
	int count = 0;

	float minX = 3.402823466e+38F;
	for (int i = 0; i < q->vl; i += 2)
	{
		minX = min(minX, q->ps[i]);
	}

	float px = q->x;
	float py = q->y;
	float linePoint1x = q->x;
	float linePoint1y = q->y;
	float linePoint2x = minX - 10;			//取最小的X值还小的值作为射线的终点
	float linePoint2y = q->y;

	float cx1, cy1, cx2, cy2;

	//printf("Polygon vert=%d: ", (q->vl) >> 1);
	//遍历每一条边
	for (int i = 0; i < q->vl; i += 2)
	{
		if (i == 0) {
			cx1 = q->ps[(q->vl - 1) << 1];
			cy1 = q->ps[((q->vl - 1) << 1) | 1];
		}
		else {
			cx1 = q->ps[i - 2];
			cy1 = q->ps[i - 1];
		}
		cx2 = q->ps[i];
		cy2 = q->ps[i + 1];
		//printf("(%f, %f) ", cy1, cy2);

		if (IsPointOnLine(px, py, cx1, cy1, cx2, cy2))
		{
			return 1;
			puts("");
		}

		if (fabs(cy2 - cy1) < 1.192092896e-07F)   //平行则不相交
		{
			continue;
		}

		if (cy1 > cy2 && IsPointOnLine(cx1, cy1, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
		{
			count++;
		}
		else if (cy2 > cy1 && IsPointOnLine(cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
		{
			count++;
		}
		else if (IsIntersect(cx1, cy1, cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
		{
			count++;
		}
	}
	//puts("");

	if (count % 2 == 1)
	{
		isInside = 1;
		printf("Point (%f, %f) is in!", q->x, q->y);
	}

	return isInside;
}

int __cdecl Point_In_Polygon_2D1(float x, float y, size_t vl, float* ps)
{
	int isInside = 0;
	int count = 0;

	float minX = 3.402823466e+38F;
	for (int i = 0; i < vl; i += 2)
	{
		minX = min(minX, ps[i]);
	}

	float px = x;
	float py = y;
	float linePoint1x = x;
	float linePoint1y = y;
	float linePoint2x = minX - 10;			//取最小的X值还小的值作为射线的终点
	float linePoint2y = y;

	float cx1, cy1, cx2, cy2;

	//printf("Polygon vert=%d: ", (vl) >> 1);
	//遍历每一条边
	for (int i = 0; i < vl; i += 2)
	{
		if (i == 0) {
			cx1 = ps[vl - 2];
			cy1 = ps[vl - 1];
		}
		else {
			cx1 = ps[i - 2];
			cy1 = ps[i - 1];
		}
		cx2 = ps[i];
		cy2 = ps[i + 1];
		//printf("(%f, %f) ", cy1, cy2);

		if (IsPointOnLine(px, py, cx1, cy1, cx2, cy2))
		{
			return 1;
			puts("");
		}

		if (fabs(cy2 - cy1) < 1.192092896e-07F)   //平行则不相交
		{
			continue;
		}

		if (cy1 > cy2 && IsPointOnLine(cx1, cy1, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
		{
			count++;
		}
		else if (cy2 > cy1 && IsPointOnLine(cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
		{
			count++;
		}
		else if (IsIntersect(cx1, cy1, cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
		{
			count++;
		}
	}
	//puts("");

	if (count % 2 == 1)
	{
		isInside = 1;
	}

	return isInside;
}



static float* tempVerts = NULL;
static int tempVertsC = 0;

__declspec(dllexport) int spSkeleton_containsPoint(spSkeleton* self, float px, float py) {
	float minX = 3.402823466e+38F;
	float minY = 3.402823466e+38F;
	float maxX = 1.175494351e-38F;
	float maxY = 1.175494351e-38F;

	for (size_t i = 0; i < self->slotsCount; ++i) {
		spSlot* slot = self->drawOrder[i];
		if (!slot->bone->active) continue;
		int verticesLength = 0;
		spAttachment* attachment = slot->attachment;

		if (attachment != NULL && attachment->type == SP_ATTACHMENT_REGION) {
			spRegionAttachment* regionAttachment = SUB_CAST(spRegionAttachment, attachment);
			verticesLength = 8;
			if (tempVertsC < verticesLength) {
				if (tempVerts != NULL) _spFree(tempVerts);
				tempVerts = CALLOC(float, verticesLength);
				tempVertsC = verticesLength;
			}
			spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, tempVerts, 0, 2);
		}
		else if (attachment != NULL && attachment->type == SP_ATTACHMENT_MESH) {
			spVertexAttachment* mesh = &(SUB_CAST(spMeshAttachment, attachment))->super;
			verticesLength = mesh->worldVerticesLength;
			if (tempVertsC < verticesLength) {
				if (tempVerts != NULL) _spFree(tempVerts);
				tempVerts = CALLOC(float, verticesLength);
				tempVertsC = verticesLength;
			}
			spVertexAttachment_computeWorldVertices(mesh, slot, 0, verticesLength, tempVerts, 0, 2);
		}
		if (Point_In_Polygon_2D1(px, py, verticesLength, tempVerts)) {
			return 1;
		}
	}
	return 0;
}
