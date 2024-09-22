#include "raycaster.h"

SDL_Renderer* renderer;

Map load_map(const char* filename) {
	Map map;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    map.total_sectors = 0;
    fscanf(file, "m %d", &map.total_sectors);
    fscanf(file, "p %f %f %f", &map.player.x, &map.player.y, &map.player.rotation);

    map.sectors = (Sector*)malloc(sizeof(Sector) * map.total_sectors);
    for (int i = 0; i < map.total_sectors; i++) {
        fscanf(file, "s %d", &map.sectors[i].total_lines);
        map.sectors[i].lines = (Line*)malloc(sizeof(Line) * map.sectors[i].total_lines);

        for (int j = 0; j < map.sectors[i].total_lines; j++) {
			int x1, y1, x2, y2;

            fscanf(file, "sd %d %d %d %d", &x1, &y1, &x2, &y2);

			map.sectors[i].lines[j].x1 = x1;
			map.sectors[i].lines[j].y1 = y1;
			map.sectors[i].lines[j].x2 = x2;
			map.sectors[i].lines[j].y2 = y2;
        }
    }

    fclose(file);
	return map;
}

Point find_intersection(Line* line, float ray_x, float ray_y, Player player, float* distance) {
    float dx = line->x2 - line->x1;
    float dy = line->y2 - line->y1;
    
    float denominator = dx * ray_y - dy * ray_x;
    if (denominator == 0) return (Point){.x = -1, .y = -1}; // Parallel

    float t = ((player.x - line->x1) * ray_y - (player.y - line->y1) * ray_x) / denominator;
    float u = ((player.x - line->x1) * dy - (player.y - line->y1) * dx) / denominator;

    if (t >= 0 && t <= 1 && u >= 0) {
        *distance = u;
        return (Point){line->x1 + t * dx, line->y1 + t * dy};
    }
    return (Point){.x = -1, .y = -1};
}

void cast_rays(Map* map, float* distances) {
    float ray_angle_step = FOV / NUM_RAYS;
    float ray_angle = map->player.rotation - FOV / 2;

    for (int i = 0; i < NUM_RAYS; i++) {
        float ray_x = cos(ray_angle);
        float ray_y = sin(ray_angle);
        float min_distance = MAX_DISTANCE;

        for (int s = 0; s < map->total_sectors; s++) {
            Sector sector = map->sectors[s];
            for (int l = 0; l < sector.total_lines; l++) {
                float distance;
                Point intersection = find_intersection(&sector.lines[l], ray_x, ray_y, map->player, &distance);
                if (intersection.x != -1 && distance < min_distance) {
                    min_distance = distance;
                }
            }
        }

        distances[i] = min_distance;
        ray_angle += ray_angle_step;
    }
}

void draw_scene(Map* map, float* distances) {
    int ceiling_color = 0xA0A0A0;
    int wall_color = 0x808080;
    int floor_color = 0x404040;
    float wall_height_ratio = 1.0f;

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        // Calculate which ray this pixel belongs to
        int ray_index = (float)x / SCREEN_WIDTH * NUM_RAYS;

        // Get the distance for the current ray
        float distance = distances[ray_index];
        float corrected_distance = distance * cos(map->player.rotation - (map->player.rotation - FOV / 2 + ray_index * FOV / NUM_RAYS));

        // Calculate the height of the wall based on distance
        float projected_wall_height = wall_height_ratio * (SCREEN_HEIGHT / corrected_distance);
        int wall_top = (SCREEN_HEIGHT / 2) - projected_wall_height / 2;
        int wall_bottom = (SCREEN_HEIGHT / 2) + projected_wall_height / 2;

        if (wall_top < 0) wall_top = 0;
        if (wall_bottom >= SCREEN_HEIGHT) wall_bottom = SCREEN_HEIGHT - 1;

        // Draw ceiling
        SDL_SetRenderDrawColor(renderer, (ceiling_color >> 16) & 0xFF, (ceiling_color >> 8) & 0xFF, ceiling_color & 0xFF, 255);
        SDL_RenderDrawLine(renderer, x, 0, x, wall_top);

        // Draw wall
        SDL_SetRenderDrawColor(renderer, (wall_color >> 16) & 0xFF, (wall_color >> 8) & 0xFF, wall_color & 0xFF, 255);
        SDL_RenderDrawLine(renderer, x, wall_top, x, wall_bottom);

        // Draw floor
        SDL_SetRenderDrawColor(renderer, (floor_color >> 16) & 0xFF, (floor_color >> 8) & 0xFF, floor_color & 0xFF, 255);
        SDL_RenderDrawLine(renderer, x, wall_bottom, x, SCREEN_HEIGHT);
    }
}

void draw_minimap(Map* map, float* distances) {
    int minimap_scale = 5;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int s = 0; s < map->total_sectors; s++) {
        Sector sector = map->sectors[s];
        for (int l = 0; l < sector.total_lines; l++) {
            Line line = sector.lines[l];
            SDL_RenderDrawLine(renderer,
                               line.x1 / minimap_scale, line.y1 / minimap_scale,
                               line.x2 / minimap_scale, line.y2 / minimap_scale);
        }
    }

    // Draw player
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawPoint(renderer, map->player.x / minimap_scale, map->player.y / minimap_scale);

    // Draw rays on minimap
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    float ray_angle_step = FOV / NUM_RAYS;
    float ray_angle = map->player.rotation - FOV / 2;
    for (int i = 0; i < NUM_RAYS; i++) {
        float ray_x = cos(ray_angle) * distances[i];
        float ray_y = sin(ray_angle) * distances[i];
        SDL_RenderDrawLine(renderer,
                           map->player.x / minimap_scale, map->player.y / minimap_scale,
                           (map->player.x + ray_x) / minimap_scale, (map->player.y + ray_y) / minimap_scale);
        ray_angle += ray_angle_step;
    }
}

