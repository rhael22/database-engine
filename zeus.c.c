#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CMD_LEN 16
#define DB_FILE_DEFAULT "telemetry.db"
#define IDX_FILE_DEFAULT "telemetry.idx"

// Pragma pack para garantir alinhamento exato de bytes na I/O
#pragma pack(push, 1)
typedef struct {
    int32_t id;
    int64_t timestamp;
    float latitude;
    float longitude;
    float temp;
} TelemetryRecord;

typedef struct {
    int32_t id;
    int64_t offset;
} IndexRecord;
#pragma pack(pop)

typedef struct {
    char db_path[256];
    char idx_path[256];
} DBContext;

static bool db_init(DBContext *ctx, const char *db_path, const char *idx_path) {
    if (!ctx || !db_path || !idx_path) return false;

    strncpy(ctx->db_path, db_path, sizeof(ctx->db_path) -1);
    strncpy(ctx->idx_path, idx_path, sizeof(ctx->idx_path) - 1);

    FILE *f_db = fopen(ctx->db_path, "a+b");
    FILE *f_idx = fopen(ctx->idx_path, "a+b");

    if (!f_db || !f_idx) {
        if (f_db) fclose(f_db);
        if (f_idx) fclose(f_idx);
        return false;
    }

    fclose(f_db);
    fclose(f_idx);
    return true;
}

static bool db_insert(const DBContext *ctx, const TelemetryRecord *record) {
    if (!ctx || !record) return false;

    FILE *f_db = fopen(ctx->db_path, "ab");
    if (!f_db) return false;

    if (fseek(f_db, 0, SEEK_END) != 0) {
        fclose(f_db);
        return false;
    }

    int64_t offset = (int64_t)ftell(f_db);

    if (fwrite(record, sizeof(TelemetryRecord), 1, f_db) != 1) {
        fclose(f_db);
        return false;
    }
    fclose(f_db);

    FILE *f_idx = fopen(ctx->idx_path, "ab");
    if (!f_idx) return false;

    IndexRecord idx_entry = { .id = record->id, .offset = offset };
    if (fwrite(&idx_entry, sizeof(IndexRecord), 1, f_idx) != 1) {
        fclose(f_idx);
        return false;
    }

    fclose(f_idx);
    return true;
}

static bool db_select(const DBContext *ctx, int32_t target_id, TelemetryRecord *out_record) {
    if (!ctx || !out_record) return false;

    FILE *f_idx = fopen(ctx->idx_path, "rb");
    if (!f_idx) return false;

    IndexRecord idx_entry;
    int64_t target_offset = -1;

    while (fread(&idx_entry, sizeof(IndexRecord), 1, f_idx) == 1) {
        if (idx_entry.id == target_id) {
            target_offset = idx_entry.offset;
            break;
        }
    }
    fclose(f_idx);

    if (target_offset == -1) return false;

    FILE *f_db = fopen(ctx->db_path, "rb");
    if (!f_db) return false;

    if (fseek(f_db, (long)target_offset, SEEK_SET) !=0) {
        fclose(f_db);
        return false;
    }

    bool success = (fread(out_record, sizeof(TelemetryRecord), 1, f_db) == 1);
    fclose(f_db);

    return success;
}


static void handle_insert(const DBContext *ctx) {
    TelemetryRecord rec;
    if (scanf("%d %ld %f %f %f", &rec.id, &rec.timestamp, &rec.latitude, &rec.longitude, &rec.temp) != 5) {
        fprintf(stderr, "Error: Invalid INSERT arguments.\n");
        while (getchar() != '\n'); // Limpa stdin
        return;
    }

    if (db_insert(ctx, &rec)) {
        printf("OK\n");
    } else {
        fprintf(stderr, "ERROR: Write operation failed.\n");
    }
}

static void handle_select(const DBContext *ctx) {
    int32_t id;
    if (scanf("%d", &id) != 1) {
        fprintf(stderr, "Error: Invalid SELECT arguments.\n");
        while (getchar() != '\n');
        return;
    }

    TelemetryRecord rec;
    if (db_select(ctx, id, &rec)) {
        printf("RECORD: id=%d ts=%ld lat=%.4f lon=%.4f temp=%.2f\n",
               rec.id, rec.timestamp, rec.latitude, rec.longitude, rec.temp);
    } else {
        fprintf(stderr, "Error: Record not found.\n");
    }
}

int main(void) {
    DBContext db_ctx;
    if (!db_init(&db_ctx, DB_FILE_DEFAULT, IDX_FILE_DEFAULT)) {
        fprintf(stderr, "Fatal: Failed to initialize database engine.\n");
        return EXIT_FAILURE;
    }

    char cmd[MAX_CMD_LEN];

    while (1) {
        printf("litedb> ");
        if (scanf("%15s", cmd) != 1) break;

        if (strcmp(cmd, "INSERT") == 0) {
            handle_insert(&db_ctx);
        } else if (strcmp(cmd, "SELECT") == 0) {
            handle_select(&db_ctx);
        } else if (strcmp(cmd, "EXIT") == 0) {
            break;
        } else {
            fprintf(stderr, "Error: Unknow command.\n");
            while (getchar() != '\n');
        }
    }

    return EXIT_SUCCESS;

}

