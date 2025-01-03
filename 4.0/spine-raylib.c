//
// Created by Pablo Weremczuk on 1/13/20.
// From https://github.com/WEREMSOFT/spine-raylib-runtimes
//
#include "spine-raylib.h"

#ifndef SP_LAYER_SPACING
#define SP_LAYER_SPACING 0
#endif

#ifndef SP_LAYER_SPACING_BASE
#define SP_LAYER_SPACING_BASE 0
#endif

float anti_z_fighting_index = SP_LAYER_SPACING_BASE;


#define MAX_TEXTURES 10
static Texture2D tm_textures[MAX_TEXTURES] = {0};
static int texture_index = 0;
static unsigned short rectangleTriangles[] = { 0, 1, 2, 2, 3, 0 };

char *_spUtil_readFile(const char *path, int *length) {
    return _spReadFile(path, length);
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
    if (self->rendererObject == NULL) return;
    Texture *t2d = self->rendererObject;
    UnloadTexture(*t2d);
}

typedef struct Vertex {
    // Position in x/y plane
    float x, y;

    // UV coordinates
    float u, v;

    // Color, each channel in the range from 0-1
    // (Should really be a 32-bit RGBA packed color)
    float r, g, b, a;
} Vertex;

#define MAX_VERTICES_PER_ATTACHMENT 2048
float worldVerticesPositions[MAX_VERTICES_PER_ATTACHMENT];

Vertex vertices[MAX_VERTICES_PER_ATTACHMENT];

void addVertex(float x, float y, float u, float v, float r, float g, float b, float a, int *index) {
    Vertex *vertex = &vertices[*index];
    vertex->x = x;
    vertex->y = y;
    vertex->u = u;
    vertex->v = v;
    vertex->r = r;
    vertex->g = g;
    vertex->b = b;
    vertex->a = a;
    *index += 1;
}

void engine_draw_region(Vertex* vertices, Texture* texture) {
    Vertex vertex;
    rlSetTexture(texture->id);
    {
        rlBegin(RL_QUADS);
        {
            rlNormal3f(0.0f, 0.0f, 1.0f);
            for (int i = 0; i < 4; i++) {
                vertex = vertices[i];
                rlColor4f(vertex.r, vertex.g, vertex.b, vertex.a);
                rlTexCoord2f(vertex.u, vertex.v);
                rlVertex2f(vertex.x, vertex.y);
            }
        }rlEnd();

#ifdef SP_DRAW_DOUBLE_FACED
        rlBegin(RL_QUADS);
        {
            rlNormal3f(0.0f, 0.0f, 1.0f);
            for (int i = 3; i >= 0; i--) {
                vertex = vertices[vertex_order[i]];
                rlTexCoord2f(vertex.u, vertex.v);
                rlColor4f(vertex.r, vertex.g, vertex.b, vertex.a);
                rlVertex3f(position.x + vertex.x, position.y + vertex.y, position.z - anti_z_fighting_index);
            }
    }rlEnd();
#endif

}
rlSetTexture(0);
}
void engine_drawMesh(Vertex* vertices, int start, int count, Texture* texture) {
    Vertex vertex;
    {
        for (int vertexIndex = start; vertexIndex < count; vertexIndex += 3) {
            rlSetTexture(texture->id);
            rlBegin(RL_QUADS);
            {
                int i;
                for (i = 2; i > -1; i--) {
                    vertex = vertices[vertexIndex + i];
                    rlTexCoord2f(vertex.u, vertex.v);
                    rlColor4f(vertex.r, vertex.g, vertex.b, vertex.a);
                    rlVertex3f(vertex.x, vertex.y, anti_z_fighting_index);
                }
                rlVertex3f(vertex.x, vertex.y, anti_z_fighting_index);
            }
            rlEnd();
#ifdef SP_DRAW_DOUBLE_FACED
            fprintf(stderr, "double sided not supported for mesh based spine files\n");
            exit(-1);
#endif

#ifdef SP_RENDER_WIREFRAME
            DrawTriangleLines((Vector2) { vertices[vertexIndex].x, vertices[vertexIndex].y },
                (Vector2) {
                vertices[vertexIndex + 1].x, vertices[vertexIndex + 1].y
            },
                (Vector2) {
                vertices[vertexIndex + 2].x, vertices[vertexIndex + 2].y
            }, vertexIndex == 0 ? RED : GREEN);
#endif

        }



    }
    rlSetTexture(0);
}

Texture2D* texture_2d_create(char* path) {
    
    Image image = LoadImage(path);

    if (image.data != NULL)
    {
		//ImageMipmaps(&image);
        tm_textures[texture_index] = LoadTextureFromImage(image);
        UnloadImage(image);
	}
	else
	{
		fprintf(stderr, "Error loading image %s\n", path);
		exit(-1);
	}

	return &tm_textures[texture_index++];
}

Texture2D* texture_2d_create1(char* path) {
    Image checked = GenImageChecked(512, 512, 1, 1, ColorFromHSV(66, 1, 1), LIGHTGRAY);
    tm_textures[texture_index] = LoadTextureFromImage(checked);
    Texture2D* t = &tm_textures[texture_index];
    SetTextureFilter(*t, TEXTURE_FILTER_BILINEAR);
    texture_index++;
    return t;
}

void texture_2d_destroy() {
    while (texture_index--) UnloadTexture(tm_textures[texture_index]);
}

void _spAtlasPage_createTexture(spAtlasPage* self, const char* path) {

    Texture2D* t = texture_2d_create((char*)path);

    // Set warp
    switch (self->uWrap) {
	case SP_ATLAS_CLAMPTOEDGE:
		SetTextureWrap(*t, TEXTURE_WRAP_CLAMP);
		break;
	case SP_ATLAS_REPEAT:
		SetTextureWrap(*t, TEXTURE_WRAP_REPEAT);
		break;
	case SP_ATLAS_MIRROREDREPEAT:
		SetTextureWrap(*t, TEXTURE_WRAP_MIRROR_REPEAT);
		break;
    default:
        exit(-ENOTRECOVERABLE);
    }

	if (self->vWrap != self->uWrap) {
		fprintf(stderr, "Warning: different wrap modes for u and v are not supported. Using u wrap mode for v\n");
	}

    self->rendererObject = t;
    if (self->width == 0) self->width = t->width;
    if (self->height == 0) self->height = t->height;
}


#define MAX_VERTICES_PER_ATTACHMENT 2048
float worldVerticesPositions[MAX_VERTICES_PER_ATTACHMENT];
Vertex vertices[MAX_VERTICES_PER_ATTACHMENT];

// Definition from glad.h
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_FUNC_ADD 0x8006

void addClippingContextVertices(spSkeletonClipping* clipping, float tintR, float tintG, float tintB, float tintA, int* vertexIndex) {
    for (int i = 0; i < clipping->clippedTriangles->size; ++i) {
        int index = clipping->clippedTriangles->items[i] << 1;
        addVertex(clipping->clippedVertices->items[index], clipping->clippedVertices->items[index + 1],
            clipping->clippedUVs->items[index], clipping->clippedUVs->items[index + 1],
            tintR, tintG, tintB, tintA, vertexIndex);
    }
}

void setBlendMode(int blend_mode, int pma) {
    switch (blend_mode)
	{
	default: //Normal
		if (pma) {
            BeginBlendMode(RL_BLEND_ALPHA_PREMULTIPLY);
		} else {
			rlSetBlendFactorsSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD, GL_FUNC_ADD);
			rlSetBlendMode(RL_BLEND_CUSTOM_SEPARATE); // actually it's an ugly patch, as Windows DMA composition needs a pma image
		}
		break;
	case 1: //Additive
        rlSetBlendFactors(pma ? GL_ONE : GL_SRC_ALPHA, GL_ONE, GL_FUNC_ADD);
        rlSetBlendMode(RL_BLEND_CUSTOM);
        break;
    case 2: //Multiply
        rlSetBlendFactors(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
        rlSetBlendMode(RL_BLEND_CUSTOM);
        break;
    case 3: //Screen
        rlSetBlendFactors(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_FUNC_ADD);
        rlSetBlendMode(RL_BLEND_CUSTOM);
        break;
    }
}

void drawSkeleton(spSkeleton* skeleton, bool PMA) {
    int blend_mode = 4; //This mode doesn't exist
    spSkeletonClipping* clipping = spSkeletonClipping_create();

    // For each slot in the draw order array of the skeleton
    anti_z_fighting_index = SP_LAYER_SPACING_BASE;
    for (int i = 0; i < skeleton->slotsCount; ++i) {
        anti_z_fighting_index -= SP_LAYER_SPACING;
        spSlot* slot = skeleton->drawOrder[i];

        // Fetch the currently active attachment, continue
        // with the next slot in the draw order if no
        // attachment is active on the slot
        spAttachment* attachment = slot->attachment;
        if (!attachment) continue;

        // Calculate the tinting color based on the skeleton's color
        // and the slot's color. Each color channel is given in the
        // range [0-1], you may have to multiply by 255 and cast to
        // and int if your engine uses integer ranges for color channels.
        float tintA = skeleton->color.a * slot->color.a;
        float alpha = PMA ? tintA : 1;
        float tintR = skeleton->color.r * slot->color.r * alpha;
        float tintG = skeleton->color.g * slot->color.g * alpha;
        float tintB = skeleton->color.b * slot->color.b * alpha;

        // Fill the vertices array depending on the type of attachment
        Texture* texture = 0;
        int vertexIndex = 0;
        if (attachment->type == SP_ATTACHMENT_REGION) {
            // Cast to an spRegionAttachment so we can get the rendererObject
            // and compute the world vertices
            spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;

            // Our engine specific Texture is stored in the spAtlasRegion which was
            // assigned to the attachment on load. It represents the texture atlas
            // page that contains the image the region attachment is mapped to
            texture = (Texture*)((spAtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;

            // Computed the world vertices positions for the 4 vertices that make up
            // the rectangular region attachment. This assumes the world transform of the
            // bone to which the slot (and hence attachment) is attached has been calculated
            // before rendering via spSkeleton_updateWorldTransform
            spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVerticesPositions, 0, 2);
            if (spSkeletonClipping_isClipping(clipping)) {
                spSkeletonClipping_clipTriangles(clipping, worldVerticesPositions, 8, rectangleTriangles, 6, regionAttachment->uvs, 2);
                addClippingContextVertices(clipping, tintR, tintG, tintB, tintA, &vertexIndex);
            } else {
                addVertex(worldVerticesPositions[0], worldVerticesPositions[1],
                    regionAttachment->uvs[0], regionAttachment->uvs[1],
                    tintR, tintG, tintB, tintA, &vertexIndex);

                addVertex(worldVerticesPositions[2], worldVerticesPositions[3],
                    regionAttachment->uvs[2], regionAttachment->uvs[3],
                    tintR, tintG, tintB, tintA, &vertexIndex);

                addVertex(worldVerticesPositions[4], worldVerticesPositions[5],
                    regionAttachment->uvs[4], regionAttachment->uvs[5],
                    tintR, tintG, tintB, tintA, &vertexIndex);

                addVertex(worldVerticesPositions[6], worldVerticesPositions[7],
                    regionAttachment->uvs[6], regionAttachment->uvs[7],
                    tintR, tintG, tintB, tintA, &vertexIndex);
            }

            if (slot->data->blendMode != blend_mode)
            {
                blend_mode = slot->data->blendMode;
                setBlendMode(blend_mode, PMA);
            }

            engine_draw_region(vertices, texture);
        }
        else if (attachment->type == SP_ATTACHMENT_MESH) {
            // Cast to an spMeshAttachment so we can get the rendererObject
            // and compute the world vertices
            spMeshAttachment* mesh = (spMeshAttachment*)attachment;

            // Check the number of vertices in the mesh attachment. If it is bigger
            // than our scratch buffer, we don't render the mesh. We do this here
            // for simplicity, in production you want to reallocate the scratch buffer
            // to fit the mesh.
            if (mesh->super.worldVerticesLength > MAX_VERTICES_PER_ATTACHMENT) continue;

            // Our engine specific Texture is stored in the spAtlasRegion which was
            // assigned to the attachment on load. It represents the texture atlas
            // page that contains the image the mesh attachment is mapped to
            texture = (Texture*)((spAtlasRegion*)mesh->rendererObject)->page->rendererObject;

            // Computed the world vertices positions for the vertices that make up
            // the mesh attachment. This assumes the world transform of the
            // bone to which the slot (and hence attachment) is attached has been calculated
            // before rendering via spSkeleton_updateWorldTransform
            spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength,
                worldVerticesPositions, 0, 2);

            if (spSkeletonClipping_isClipping(clipping)) {
                spSkeletonClipping_clipTriangles(clipping, worldVerticesPositions, mesh->super.worldVerticesLength, mesh->triangles, mesh->trianglesCount, mesh->uvs, 2);
                addClippingContextVertices(clipping, tintR, tintG, tintB, tintA, &vertexIndex);
            } else {
                // Mesh attachments use an array of vertices, and an array of indices to define which
                // 3 vertices make up each triangle. We loop through all triangle indices
                // and simply emit a vertex for each triangle's vertex.
                for (int i = 0; i < mesh->trianglesCount; ++i) {
                    int index = mesh->triangles[i] << 1;
                    addVertex(worldVerticesPositions[index], worldVerticesPositions[index + 1],
                        mesh->uvs[index], mesh->uvs[index + 1],
                        tintR, tintG, tintB, tintA, &vertexIndex);
                }
            }
            
            if (slot->data->blendMode != blend_mode)
            {
                blend_mode = slot->data->blendMode;
                setBlendMode(blend_mode, PMA);
            }
            // Draw the mesh we created for the attachment
            engine_drawMesh(vertices, 0, vertexIndex, texture);
        }
        else if (attachment->type == SP_ATTACHMENT_CLIPPING) {
            spSkeletonClipping_clipStart(clipping, slot, (spClippingAttachment*)attachment);
        }

        spSkeletonClipping_clipEnd(clipping, slot);
    }
    spSkeletonClipping_dispose(clipping);
    EndBlendMode(); //Exit out
}
