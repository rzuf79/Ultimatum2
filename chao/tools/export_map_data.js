#!/usr/bin/env node

const fs = require("fs");
const path = require("path");
const vm = require("vm");

const repoRoot = path.resolve(__dirname, "..", "..");
const jsRoot = path.join(repoRoot, "js");
const outputPath = path.join(repoRoot, "chao", "ultimatum", "generated_map_data.h");

function loadScript(fileName, exportName, context) {
    const fullPath = path.join(jsRoot, fileName);
    const source = fs.readFileSync(fullPath, "utf8");
    const wrappedSource = `${source}\n\nglobalThis.__exports = globalThis.__exports || {};\nglobalThis.__exports.${exportName} = ${exportName};\n`;
    new vm.Script(wrappedSource, { filename: fullPath }).runInContext(context);
}

function quoteCString(value) {
    return `"${String(value).replace(/\\/g, "\\\\").replace(/"/g, '\\"')}"`;
}

function toCRgba(jsColor) {
    const value = Number(jsColor) >>> 0;
    const a = (value >>> 24) & 0xFF;
    const r = (value >>> 16) & 0xFF;
    const g = (value >>> 8) & 0xFF;
    const b = value & 0xFF;
    const rgba = (((r << 24) >>> 0) | (g << 16) | (b << 8) | a) >>> 0;
    return `0x${rgba.toString(16).toUpperCase().padStart(8, "0")}u`;
}

function toCCharLiteral(symbol) {
    const value = symbol.charCodeAt(0);
    return `(char)0x${value.toString(16).toUpperCase().padStart(2, "0")}`;
}

function assert(condition, message) {
    if (!condition) {
        throw new Error(message);
    }
}

const context = vm.createContext({ console });
loadScript("Reg.js", "Reg", context);
loadScript("Tiles.js", "Tiles", context);
loadScript("Tilesets.js", "Tilesets", context);
loadScript("Maps.js", "Maps", context);

const { Tiles, Tilesets, Maps } = context.__exports;
assert(Tiles, "Failed to load Tiles.js");
assert(Tilesets, "Failed to load Tilesets.js");
assert(Maps, "Failed to load Maps.js");

const tileset = Tilesets.getCurrent();
const palette = tileset.palette;
const tintedTiles = tileset.colors || {};
const symbolEntries = Object.entries(Maps.symbols);
const mapEntries = Object.entries(Maps).filter(([mapId]) => mapId !== "symbols");

const lines = [];
lines.push("#ifndef GENERATED_MAP_DATA_H");
lines.push("#define GENERATED_MAP_DATA_H");
lines.push("");
lines.push("#include <stdbool.h>");
lines.push("#include <stddef.h>");
lines.push("#include <stdint.h>");
lines.push("#include <string.h>");
lines.push("");
lines.push("enum { U2_TILE_SIZE = 16 };");
lines.push("");
lines.push("typedef struct {");
lines.push("    const char* name;");
lines.push("    char symbol;");
lines.push("    int atlas_x;");
lines.push("    int atlas_y;");
lines.push("    uint32_t tint;");
lines.push("    bool passable;");
lines.push("} U2TileDef;");
lines.push("");
lines.push("typedef struct {");
lines.push("    const char* id;");
lines.push("    const char* name;");
lines.push("    bool looped;");
lines.push("    int width;");
lines.push("    int height;");
lines.push("    const char* const* rows;");
lines.push("} U2MapDef;");
lines.push("");
lines.push("static const U2TileDef u2_tile_defs[] = {");

for (const [tileName, symbol] of symbolEntries) {
    const tile = Tiles[tileName];
    assert(tile, `Missing tile metadata for "${tileName}"`);
    const tintIndex = tintedTiles[tileName] || 0;
    const tint = palette[tintIndex];
    assert(typeof tint === "number", `Missing tint palette entry for "${tileName}"`);

    lines.push(
        `    { ${quoteCString(tileName)}, ${toCCharLiteral(symbol)}, ${tile.x}, ${tile.y}, ${toCRgba(tint)}, ${tile.passable ? "true" : "false"} },`
    );
}

lines.push("};");
lines.push("");
lines.push("static const size_t u2_tile_defs_count = sizeof(u2_tile_defs) / sizeof(u2_tile_defs[0]);");
lines.push("");

for (const [mapId, mapData] of mapEntries) {
    assert(Array.isArray(mapData.map), `Map "${mapId}" is missing rows`);
    const width = mapData.map[0] ? mapData.map[0].length : 0;
    const height = mapData.map.length;

    for (const row of mapData.map) {
        assert(row.length === width, `Map "${mapId}" has inconsistent row widths`);
    }

    lines.push(`static const char* const u2_map_${mapId}_rows[] = {`);
    for (const row of mapData.map) {
        lines.push(`    ${quoteCString(row)},`);
    }
    lines.push("};");
    lines.push("");
    lines.push(`static const U2MapDef u2_map_${mapId} = { ${quoteCString(mapId)}, ${quoteCString(mapData.name)}, ${mapData.looped ? "true" : "false"}, ${width}, ${height}, u2_map_${mapId}_rows };`);
    lines.push("");
}

lines.push("static const U2MapDef* const u2_maps[] = {");
for (const [mapId] of mapEntries) {
    lines.push(`    &u2_map_${mapId},`);
}
lines.push("};");
lines.push("");
lines.push("static const size_t u2_maps_count = sizeof(u2_maps) / sizeof(u2_maps[0]);");
lines.push("");
lines.push("static inline const U2MapDef* u2_find_map(const char* id) {");
lines.push("    for (size_t i = 0; i < u2_maps_count; ++i) {");
lines.push("        if (strcmp(u2_maps[i]->id, id) == 0) {");
lines.push("            return u2_maps[i];");
lines.push("        }");
lines.push("    }");
lines.push("    return NULL;");
lines.push("}");
lines.push("");
lines.push("static inline const U2TileDef* u2_find_tile_by_symbol(char symbol) {");
lines.push("    for (size_t i = 0; i < u2_tile_defs_count; ++i) {");
lines.push("        if (u2_tile_defs[i].symbol == symbol) {");
lines.push("            return &u2_tile_defs[i];");
lines.push("        }");
lines.push("    }");
lines.push("    return NULL;");
lines.push("}");
lines.push("");
lines.push("#endif // GENERATED_MAP_DATA_H");

fs.writeFileSync(outputPath, `${lines.join("\n")}\n`);
console.log(`Wrote ${outputPath}`);
